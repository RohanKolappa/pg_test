package com.ipvsserv.common.util;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Random;
import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;

public class IpvsDBUtils {
    private static final String MRSCHEMAPATH = "/com/ipvs/common/service/MRSchema.xml";
    public static final String NODEID_2 = "NID";
    public static final String PARENT_NODEID = "parentNID";
    public static final Random wheel = new Random();
    private static boolean inited=false;
    private static Hashtable<String, String> rootPrefixTable = null;
    private static Hashtable<String, String> parentListTable = null;
    static {
        initMRSchema();
    }
    public static String getNewParentNID(String serviceDomain, String objectType) {
        try {
            MRConfig config = MRConfig.getInstance(objectType);
            String parentKey = config.getParentNIDKey();    
            //String listName= parentListTable.get(parentKey);
            return serviceDomain + "." + parentKey.toLowerCase();
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        return serviceDomain + "." + "unknown_" + objectType.toLowerCase() + "list";
    }

    public static String getNewNID(String serviceDomain, String objectType) {
        return getAutoNID(getNewParentNID(serviceDomain, objectType));
    }

    public static String getAutoNID(String parentNID) {
        NIDSchemeMDC nidScheme = new NIDSchemeMDC();
        nidScheme.parse(parentNID);
        if (nidScheme.getType() != NIDSchemeMDC.COLLECTION) {
            MRLog.error("getAutoNID Problem ????????1111????? parentNID  " + parentNID);
            //throw new Exception("Invalid list parentNID ='" +parentNID+"'");
        }
        String listPrefix = getListPrefix(nidScheme.getCollectionId());
        String NID = listPrefix + "_" + getUUID();
        return nidScheme.getCollectionNodeID() + "." + NID.toLowerCase();
    }

    ///////////////////////
    private static void initRoots(Hashtable<String, String> rootTable, Element el) throws Exception {
        String elementName = el.getName();
        if (el.isRootElement()) {
            rootTable.put(elementName, "");
        } else {
            rootTable.put(elementName, el.getParent().getName().toLowerCase());
        }
        List<?> childList = Dom4jUtils.getList(el);
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childReqEL = (Element) i.next();
            initRoots(rootTable, childReqEL);
        }
    }

    ///////////////////////

    private static void initMRSchema()  {
        try {
            String schemaPath = MRSCHEMAPATH;
            Document schemadoc = Dom4jUtils.loadXMLResource(schemaPath);
            rootPrefixTable = new Hashtable<String, String>();
            parentListTable = new Hashtable<String, String>();
            initDBListPrefix(rootPrefixTable, schemadoc.getRootElement());
            initRoots(parentListTable, schemadoc.getRootElement());
            inited=true;
        } catch (Exception ex) {
            ex.printStackTrace();
        }        
    }

    private static void initDBListPrefix(Hashtable<String, String> rootsPrefix, Element el) throws Exception {
        String elementName = el.getName();
        String elementNamePrefix = createListPrefix(elementName);
        rootsPrefix.put(elementName.toLowerCase(), elementNamePrefix);
        List<?> childList = Dom4jUtils.getList(el);
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element childReqEL = (Element) i.next();
            initDBListPrefix(rootsPrefix, childReqEL);
        }
    }

    private static String createListPrefix(String elementName) {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < elementName.length(); i++) {
            //65 - 90
            char c = elementName.charAt(i);
            if ((c < 'Z' && c > 'A') || c == 'A' || c == 'Z') {
                buf.append(c);
            }
        }
        return buf.toString();
    }

    private static String getListPrefix(String key) {
        if (rootPrefixTable == null || !rootPrefixTable.containsKey(key))
            return "zzzz_";
        return rootPrefixTable.get(key);
    }

    public static String getUUID() {
        UUID objUUID = UUID.randomUUID();

        return objUUID.toString();
    }

    public static String parseNID_BULK_SCHEMA(Element element, String serviceDomain) throws Exception {
        // Find if its new method
        String elementName = element.getName();
        String NID = serviceDomain + "." + elementName;
        Dom4jUtils.addAttribute(element, NODEID_2, NID.toLowerCase());
        List<?> childList = Dom4jUtils.getList(element);
        for (Iterator<?> i = childList.iterator(); i.hasNext();) {
            Element toELChild = (Element) i.next();
            parseNID_BULK_SCHEMA(toELChild, serviceDomain);
        }
        return NID;
    }

    public static String parseNID_BULK(Element element, String parentNID) throws Exception {
        // Find if its new method
        String NID = Dom4jUtils.getAttributeValue(element, NODEID_2, "").trim();
        if (!NID.trim().equals("")) {
            return NID;
        }
        return getAutoNID(parentNID);
    }

    public static void applyNID(Element element, String parentNID, int loop) throws Exception {
        try {
            String NID = IpvsDBUtils.parseNID_BULK(element, parentNID);
            Dom4jUtils.addAttribute(element, NODEID_2, NID);
            Dom4jUtils.addAttribute(element, PARENT_NODEID, parentNID);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

    public static void applyNID(Element element, String parentNID) throws Exception {
        try {
            String NID = IpvsDBUtils.parseNID_BULK(element, parentNID);
            Dom4jUtils.addAttribute(element, NODEID_2, NID);
            Dom4jUtils.addAttribute(element, PARENT_NODEID, parentNID);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
}
