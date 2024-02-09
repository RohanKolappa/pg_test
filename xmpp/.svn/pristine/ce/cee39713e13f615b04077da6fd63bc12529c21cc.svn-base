package com.ipvsserv.common.util;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
 

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

    private static Hashtable<String,String> objectServiceMappings = new Hashtable<String,String>();
    static {
        try{
            loadConfigDoclist(xmlFilePath);
        }catch(Exception ex){
            ex.printStackTrace();
        }
    }
    // ///////// TBD:Singleton object ///////////////////////
    private MRConfig(String serviceName, String dbObjectName) {
        this.serviceName = serviceName;
        this.dbObjectName = dbObjectName;
    }
    private synchronized void initialize(String serviceName, String dbObjectName) throws Exception {
        Document configDoc = null;
        if(serviceName==null || serviceName.equals("") || !configDoclist.containsKey(serviceName)){
            serviceName = objectServiceMappings.get(dbObjectName);
        }
        if (configDoclist == null || !configDoclist.containsKey(serviceName)) {
            throw new Exception(xmlFilePath);
        } else {
            configDoc = configDoclist.get(serviceName);
        }

        if (configEL == null) { // set configEL
            String xpath = "//DbObjectConfig[@name='" + dbObjectName + "']";
            configEL = Dom4jUtils.getElementXpath(configDoc, xpath);
        }
        if (configDoc == null || configEL == null) {
            throw new Exception(serviceName + " "+ dbObjectName);
        }
    }   
    // ///////// getInstance ///////////////////////
    public synchronized static MRConfig getInstance(String dbObjectName) throws Exception {
        return getInstance(null, dbObjectName);
    }
    public synchronized static MRConfig getInstance(String serviceName, String dbObjectName) throws Exception {
        if (list == null) {
            list = new Hashtable<String, MRConfig>();
        }
        if (configDoclist == null) {
            loadConfigDoclist(xmlFilePath);
        }
        MRConfig config = null;
        if (!list.containsKey(dbObjectName)) {
            config = new MRConfig(serviceName, dbObjectName);
            config.initialize(serviceName, dbObjectName);
            list.put(dbObjectName, config);
        }
        config = list.get(dbObjectName);
        if (config != null) // tbd to remove
            config.initialize(serviceName,dbObjectName);
        return config;
    }

    ////////////////////////// for object list ///////////////
    private synchronized static void loadConfigDoclist(String pxmlFilePath) throws Exception {
        xmlFilePath = pxmlFilePath;
        configDoclist = new Hashtable<String, Document>();
        Document configDoc = Dom4jUtils.loadXMLResource(xmlFilePath);
        List<?> childList = Dom4jUtils.getList(configDoc.getRootElement());
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childEL = (Element) i.next();
            try {
                String serviceName = Dom4jUtils.getAttributeValue(childEL, "serviceName", "").trim();
                String loadFrom = Dom4jUtils.getAttributeValue(childEL, "loadFrom", "JAR").trim();
                String file = Dom4jUtils.getAttributeValue(childEL, "file", "").trim();
                if (!file.equals("")) {
                    Document doc = null;
                    if (loadFrom.equals("JAR")) {
                        doc = Dom4jUtils.loadXMLResource(file);
                    } else if (loadFrom.equals("FILE")) {
                        doc = Dom4jUtils.loadXMLFile(file);
                    }
                    configDoclist.put(serviceName, doc);
                    parseObjectList(serviceName, doc);
                } else {
                    //System.out.println("loadConfigDoclist file is mepty " + pxmlFilePath);
                }

            } catch (Exception ex) {
                ex.printStackTrace(); // neglect error
            }
        }
    }
    private synchronized static void parseObjectList(String serviceName, Document doc) throws Exception {
        try {
             List<?> list= Dom4jUtils.getList(doc, "DbObjectConfig");
             for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                 Element el = (Element) iterator.next();
                 String objectName = Dom4jUtils.getAttributeValue(el, "name", "");
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
   
    private String getConfigParam(String parentElementName, String attibutename, String defaultValue) throws Exception {
        String value = Dom4jUtils.getAttributeValue(Dom4jUtils.element(configEL, parentElementName), attibutename, defaultValue).trim();
        return value;
    }
    /////////////////////////////////
    
    public String getDbObjectName() throws Exception {
        return dbObjectName;
    }
    public String getParentNIDKey() throws Exception {
        if (parentNIDKey == null || parentNIDKey.equals("")) {
            // <ParentNID schemaKey="MediaStreamSrcPortResourceRoot"/>
            parentNIDKey = getConfigParam("ParentNID", "schemaKey", "");
        }
        return parentNIDKey;
    }
    
    public String getType() {
        if (type == null || type.equals("")) {
            // <DbObjectConfig name="DeviceList" type="LIST" remark="Used only for GetRequest Action">
            type = Dom4jUtils.getAttributeValue(configEL, "type", "").trim();
        }
        return type;
    }

    public void setParentNIDKey(String parentNIDKey) {
        this.parentNIDKey = parentNIDKey;
    }

    
    
}
