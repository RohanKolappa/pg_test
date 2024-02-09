package com.ipvsserv.nxd.xslt.extn;


public class MyCounter {
//    static Hashtable counters = new Hashtable();
//
//    public void init(org.apache.xalan.extensions.XSLProcessorContext context, org.w3c.dom.Element elem) {
//        String name = elem.getAttribute("name");
//        String value = elem.getAttribute("value");
//        int val;
//        try {
//            val = Integer.parseInt(value);
//        } catch (NumberFormatException e) {
//            e.printStackTrace();
//            val = 0;
//        }
//        counters.put(name, new Integer(val));
//    }
//
//    public void incr(org.apache.xalan.extensions.XSLProcessorContext context, org.w3c.dom.Element elem) {
//        String name = elem.getAttribute("name");
//        Integer cval = (Integer) counters.get(name);
//        int nval = (cval == null) ? 0 : (cval.intValue() + 1);
//        counters.put(name, new Integer(nval));
//    }
//
//    public int read(String name) {
//        Integer cval = (Integer) counters.get(name);
//        return (cval == null) ? 0 : cval.intValue();
//    }
//
//    public int read2(Object elem) {
//        DTMNodeIterator mDTMNodeIterator = (DTMNodeIterator) elem;
//        MRLog.debug(" read2 " + DOMXmlUtils.elementToString(mDTMNodeIterator.getRoot()));
//        return 1;
//    }
//
//    public int read3(Object elem) {
//
//        DTMNodeIterator mDTMNodeIterator = (DTMNodeIterator) elem;
//        MRLog.debug(" read3 " + DOMXmlUtils.elementToString(mDTMNodeIterator.getRoot()));
//        return 1;
//    }
//
//    public static boolean doSomething(org.w3c.dom.NodeList nList) {
//
//        //DTMNodeIterator mDTMNodeIterator = (DTMNodeIterator) elem;
//        MRLog.debug(" read3 " + DOMXmlUtils.elementToString(nList.item(0)));
//        return true;
//    }
//
//    ////////
//    public static boolean isDMSUserRegistrationNeeded(String user, String password, String assetResource, String deviceType) {
//        try {
//
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return false;
//    }
//
//    public static String newNID(String elementName, String serviceDomain) {
//        try {
//
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    public static String newParentNID(String elementName, String serviceDomain) {
//        try {
//
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return "";
//    }
//
//    ///////////
//    public static boolean printdocument(org.w3c.dom.NodeList nList) {
//        //DTMNodeIterator mDTMNodeIterator = (DTMNodeIterator) elem;
//        MRLog.debug(" printdocument " + DOMXmlUtils.elementToString(nList.item(0)));
//        return true;
//    }
//
//    public static Document changeXpathValue(org.w3c.dom.NodeList nList, String xpath, String value) {
//        Document doc = null;
//        try {
//            Node el = nList.item(0);
//            String xml = DOMXmlUtils.elementToString(el);
//            xml = Dom4jXsltUtils.setValue(xml, xpath, value);
//            doc = DOMXmlUtils.stringToDocument(xml);
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return doc;
//    }
//
//    public static String addDocument(org.w3c.dom.NodeList nList, String xpath, String value) {
//        String nid = "";
//        try {
//            Node el = nList.item(0);
//            String xml = DOMXmlUtils.elementToString(el);
//            MRLog.debug("Querying DOM using " + xml);
//            xml = Dom4jXsltUtils.addDocument_DB(xml, xpath, value);
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return nid;
//    }
//
//    public static boolean updateDocument(org.w3c.dom.NodeList nList, String xpath, String value) {
//        try {
//            Node el = nList.item(0);
//            String xml = DOMXmlUtils.elementToString(el);
//            MRLog.debug("Querying DOM using " + xml);
//            xml = Dom4jXsltUtils.addDocument_DB(xml, xpath, value);
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return true;
//    }
//
//    public static boolean deleteDocument(org.w3c.dom.NodeList nList, String xpath, String value) {
//        try {
//            Node el = nList.item(0);
//            String xml = DOMXmlUtils.elementToString(el);
//            MRLog.debug("Querying DOM using " + xml);
//            xml = Dom4jXsltUtils.addDocument_DB(xml, xpath, value);
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return true;
//    }
//
//    public static Document readDocument(org.w3c.dom.NodeList nList, String xpath, String value) {
//        Document doc = null;
//        try {
//            Node el = nList.item(0);
//            String xml = DOMXmlUtils.elementToString(el);
//            xml = Dom4jXsltUtils.setValue(xml, xpath, value);
//            doc = DOMXmlUtils.stringToDocument(xml);
//        } catch (Exception e) {
//            e.printStackTrace();
//        }
//        return doc;
//    }

}
