package com.ipvs.common.db;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;

public class MRConfig {
    private static Hashtable<String, MRConfig> list = null;
    private static String xmlFilePath = "/com/ipvs/common/db/MRConfig.xml";
    private static Hashtable<String, Document> configDoclist = null;

    // config object
    protected String dbObjectName = null;
    protected String serviceName = "";

    protected Element configEL = null;
    // config params
    protected String parentNIDKey = "";
    protected String type = "";

    //protected ConfigHandler configHandler = null;
    protected CustomValidation customValidation = null;
    //protected DefaultValuePatchupXml defaultValuePatchupXml = null;
    protected XmlRepresentation xmlRepresentation = null;
    //protected XsdValidation xsdValidation = null;
    // map for object type and service name
    private static Hashtable<String,String> objectServiceMappings = new Hashtable<String,String>();
    static {
        try{
            loadConfigDoclist(xmlFilePath, MRLog.getInstance(MRLog.CONSOLE));
        }catch(Exception ex){
            ex.printStackTrace();
        }
    }
    // ///////// TBD:Singleton object ///////////////////////
    private MRConfig(String serviceName, String dbObjectName) {
        this.serviceName = serviceName;
        this.dbObjectName = dbObjectName;
    }
    private synchronized void initialize(String serviceName, String dbObjectName, MRLog log) throws Exception {
        Document configDoc = null;
        if(serviceName==null || serviceName.equals("") || !configDoclist.containsKey(serviceName)){
            serviceName = objectServiceMappings.get(dbObjectName);
        }
        if (configDoclist == null || !configDoclist.containsKey(serviceName)) {
            throw new MRException(DBExceptionCodes.NO_MAPPING_FOUND, MRXMLUtils.generateErrorXMLTags("serviceName",serviceName,"xmlFilePath",xmlFilePath));
        } else {
            configDoc = configDoclist.get(serviceName);
        }

        if (configEL == null) { // set configEL
            String xpath = "//DbObjectConfig[@name='" + dbObjectName + "']";
            configEL = MRXMLUtils.getElementXpath(configDoc, xpath);
        }
        if (configDoc == null || configEL == null) {
            throw new MRException(DBExceptionCodes.NO_MAPPING_FOUND, MRXMLUtils.generateErrorXMLTags("serviceName",serviceName,"xmlFilePath",dbObjectName));
        }
    }   
    // ///////// getInstance ///////////////////////
    public synchronized static MRConfig getInstance(String dbObjectName, MRLog log) throws Exception {
        return getInstance(null, dbObjectName, log);
    }
    public synchronized static MRConfig getInstance(String serviceName, String dbObjectName, MRLog log) throws Exception {
        if (list == null) {
            list = new Hashtable<String, MRConfig>();
        }
        if (configDoclist == null) {
            loadConfigDoclist(xmlFilePath, log);
        }
        MRConfig config = null;
        if (!list.containsKey(dbObjectName)) {
            config = new MRConfig(serviceName, dbObjectName);
            config.initialize(serviceName, dbObjectName,log);
            list.put(dbObjectName, config);
        }
        config = list.get(dbObjectName);
        if (config != null) // tbd to remove
            config.initialize(serviceName,dbObjectName,log);
        return config;
    }

    ////////////////////////// for object list ///////////////
    private synchronized static void loadConfigDoclist(String pxmlFilePath, MRLog log) throws Exception {
        xmlFilePath = pxmlFilePath;
        configDoclist = new Hashtable<String, Document>();
        Document configDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        List<?> childList = MRXMLUtils.getList(configDoc.getRootElement());
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childEL = (Element) i.next();
            try {
                String serviceName = MRXMLUtils.getAttributeValue(childEL, "serviceName", "").trim();
                String loadFrom = MRXMLUtils.getAttributeValue(childEL, "loadFrom", "JAR").trim();
                String file = MRXMLUtils.getAttributeValue(childEL, "file", "").trim();
                if (!file.equals("")) {
                    Document doc = null;
                    if (loadFrom.equals("JAR")) {
                        doc = MRXMLUtils.loadXMLResource(file, log);
                    } else if (loadFrom.equals("FILE")) {
                        doc = MRXMLUtils.loadXMLFile(file, log);
                    }
                    configDoclist.put(serviceName, doc);
                    parseObjectList(serviceName, doc, log);
                } else {
                    //System.out.println("loadConfigDoclist file is mepty " + pxmlFilePath);
                }

            } catch (Exception ex) {
                ex.printStackTrace(); // neglect error
            }
        }
    }
    private synchronized static void parseObjectList(String serviceName, Document doc, MRLog log) throws Exception {
        try {
             List<?> list= MRXMLUtils.getList(doc, "DbObjectConfig");
             for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                 Element el = (Element) iterator.next();
                 String objectName = MRXMLUtils.getAttributeValue(el, "name", "");
                 objectServiceMappings.put(objectName, serviceName);
             }
          } catch (Exception ex) {
              ex.printStackTrace(); // neglect error
          }
     }
    public static String getObjectServiceName(String objectName) throws Exception {
        return objectServiceMappings.get(objectName);
    }
    public static Hashtable<String,String> getObjectServiceMappings() throws Exception {
        return objectServiceMappings;
    }
    ////////////////////////// for object list ///////////////


 
    /////////////////////////////////
    private synchronized Document getConfigDoc(String name, MRLog log) throws Exception {
        Document doc = null;
        if (doc == null) {
            // <ValidationXsd loadFrom="JAR"  file="/src/com/ipvs/xsd/mediaroom.assetadminservice.datamodel.Device.xsd"  />
            String file = getConfigParam(name, "file", "", log).trim();
            String loadFrom = getConfigParam(name, "loadFrom", "", log).trim();
            if(file.equals(""))
                return null;
            
            if (loadFrom.equalsIgnoreCase("JAR")) {
                doc = MRXMLUtils.loadXMLResource(file, log);
            } else if (loadFrom.equalsIgnoreCase("FILE")) {
                doc = MRXMLUtils.loadXMLFile(file, log);
            
            } else if (loadFrom.equalsIgnoreCase("OBJECTS")) {
                String createRootName = getConfigParam(name, "createRootName", "", log);
                Element xmlRepEL = MRXMLUtils.getElement(configEL, name);
                doc = MRXMLUtils.getDocument();
                Element root = MRXMLUtils.addElement(doc, createRootName);
                if (xmlRepEL != null) {
                    List<?> childList = MRXMLUtils.getList(xmlRepEL);
                    for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                        Element childEL = (Element) i.next();
                        try {
                            String refObject = MRXMLUtils.getAttributeValue(childEL, "name", "").trim();
                            //System.out.println("refObject: " + refObject);
                            if (!refObject.equals("")) {
                                MRConfig amConfig = MRConfig.getInstance(serviceName, refObject, log);
                                if (amConfig != null) {
                                    Document docTemp = amConfig.getXmlRepresentation(log).getDoc();
                                    if (docTemp != null) {
                                        Element rootTemp = docTemp.getRootElement();
                                        if (rootTemp != null) {
                                            MRXMLUtils.addElement(root, (Element) rootTemp.clone());
                                        }
                                    }
                                }
                            }
                        } catch (Exception ex) {
                            ex.printStackTrace(); // neglect error
                        }
                    }
                }
                 
            } else {
                throw new MRException(DBExceptionCodes.INCOMPLETE_CONFIGURATION_FOUND, MRXMLUtils.generateErrorXML("name",name));
            }
        }
        return doc;
    }    
    private String getConfigParam(String parentElementName, String attibutename, String defaultValue, MRLog log) throws Exception {
        String value = MRXMLUtils.getAttributeValue(configEL, parentElementName, attibutename, defaultValue).trim();
        return value;
    }
    /////////////////////////////////
    
    public String getDbObjectName(MRLog log) throws Exception {
        return dbObjectName;
    }
    public String getParentNIDKey(MRLog log) throws Exception {
        if (parentNIDKey == null || parentNIDKey.equals("")) {
            // <ParentNID schemaKey="MediaStreamSrcPortResourceRoot"/>
            parentNIDKey = getConfigParam("ParentNID", "schemaKey", "", log);
        }
        return parentNIDKey;
    }
    
    public String getType() {
        if (type == null || type.equals("")) {
            // <DbObjectConfig name="DeviceList" type="LIST" remark="Used only for GetRequest Action">
            type = MRXMLUtils.getAttributeValue(configEL, "type", "").trim();
        }
        return type;
    }

    public void setParentNIDKey(String parentNIDKey, MRLog log) {
        this.parentNIDKey = parentNIDKey;
    }

    public synchronized XmlRepresentation getXmlRepresentation(MRLog log) throws Exception {
        if (xmlRepresentation == null) {
            // <XmlRepresentation loadFrom="JAR" file="/src/com/ipvs/xsd/objectXml/MediaStreamSrcPortResource.xml" />
            Document doc = getConfigDoc("XmlRepresentation", log);
            xmlRepresentation = new XmlRepresentation();
            xmlRepresentation.setDoc(doc);
        }
        return xmlRepresentation;
    }
 
    public synchronized CustomValidation getCustomValidation(MRLog log) throws Exception {
        if (customValidation == null) {
            // <ConfigHandlerXml loadFrom="JAR" file=" /src/com/ipvs/xsd/xml/MediaStreamSrcPortResource_confighandler.xml" />
            Document doc = getConfigDoc("CustomValidationXml", log);
            customValidation = new CustomValidation();
            customValidation.setDoc(doc);
        }
        return customValidation;
    }

    public  static Document  getObjectXMLDocument (String serviceName, String dbObjectName,MRLog log) throws MRException,Exception {
        if (dbObjectName == null || dbObjectName.equals("")){
            return null;
        }
        MRConfig mrConfig = MRConfig.getInstance(serviceName, dbObjectName, log);
        return (Document) mrConfig.getXmlRepresentation(log).getDoc().clone();     
    }
}
