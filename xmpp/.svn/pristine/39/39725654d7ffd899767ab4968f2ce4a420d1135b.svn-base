package com.ipvsserv.common.util;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.StringWriter;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.dom4j.CDATA;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.Node;
import org.dom4j.QName;
import org.dom4j.XPath;
import org.dom4j.io.DOMWriter;
import org.dom4j.io.OutputFormat;
import org.dom4j.io.SAXReader;
import org.dom4j.io.XMLWriter;
import org.xml.sax.InputSource;

import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class Dom4jUtils {
    /**
     * Pool of SAX Readers. SAXReader is not thread safe so we need to have a pool of readers.
     */
    private static BlockingQueue<SAXReader> xmlReaders = new LinkedBlockingQueue<SAXReader>();
    static {
        // Initialize the pool of sax readers
        for (int i = 0; i < 50; i++) {
            SAXReader xmlReader = new SAXReader();
            // xmlReader.setEncoding("UTF-8");
            Dom4jUtils.xmlReaders.add(xmlReader);
        }
    }
    
    public static void checkInSAXReader(SAXReader xmlReader) {
        Dom4jUtils.xmlReaders.add(xmlReader);
    }
    
    public static SAXReader checkOutSAXReader() throws InterruptedException {
        SAXReader xmlReader = Dom4jUtils.xmlReaders.take();
        return xmlReader;
    }
    public static void removeNameSpace(Element itemData) {
        try {
            itemData.remove(itemData.getNamespace());
            itemData.getNamespace().detach();
            itemData.setQName(QName.get(itemData.getName(), Namespace.NO_NAMESPACE));
            List<?> list = itemData.elements();
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element object = (Element) iterator.next();
                removeNameSpace(object);
            } 
        } catch (Exception ex) {
        }
    }
    public static Document stringToDocument(String xml) {
        if (xml == null) {
            return null;
        }
        SAXReader xmlReader = null;
        try {
            xmlReader = Dom4jUtils.checkOutSAXReader();
            return xmlReader.read(new StringReader(xml));
        } catch (Exception ex) {
            MRLog.error("stringToDocument_xml:" + xml + ":stringToDocument_xml");
            MRLog.error(ex);
        } finally {
            Dom4jUtils.checkInSAXReader(xmlReader);
        }
        return null;
    }
    
    public static Element stringToElement(String xml) {
        Document doc = Dom4jUtils.stringToDocument(xml);
        if (doc != null) {
            return doc.getRootElement();
        }
        return null;
    }
    
    public static String documentToString(Document doc) {
        if (doc == null) {
            return null;
        }
        try {
            if(doc.getRootElement() !=null)
            return doc.getRootElement().asXML();
        } catch (Exception e) {
        }
        try {
            return doc.asXML();
        } catch (Exception e) {
        }
        return "";
    }
    
    /* public static String elementToString(Element el) {
         if (el != null) {
             try {
                 StringWriter os = new StringWriter();
                 OutputFormat format = OutputFormat.createCompactFormat();// new
                 // OutputFormat
                 // (
                 // "0"
                 // ,
                 // false
                 // )
                 // ;
                 // /
                 // /
                 // ,
                 // "UTF-8"
                 // )
                 // ;
                 XMLWriter writer = new XMLWriter(os, format);
                 writer.write(el);
                 return os.toString();
             } catch (IOException ex) {
                 MRLog.error(ex);
             }
         }
         return "";
     }*/

    public static org.w3c.dom.Document createW3CDOM(Document doc) {
        if (doc == null) {
            return null;
        }
        try {
            DOMWriter dw = new DOMWriter();
            return dw.write(doc);
        } catch (Exception de) {}
        return null;
    }
    
    public static Document getDocument() {
        return DocumentHelper.createDocument();
    }
    
    public static Document getDocument(File file) {
        try {
            SAXReader xmlReader = new SAXReader();
            return xmlReader.read(file);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    // ////////////////
    public static void saveToFile(Document document, String filepath) throws IOException {
        // long start = System.currentTimeMillis();
        try {
            OutputFormat format = OutputFormat.createPrettyPrint();
            XMLWriter writer = new XMLWriter(new FileWriter(filepath), format);
            writer.write(document);
            writer.close();
        } catch (IOException ex) {
            MRLog.error(ex);
        }
        // long end = System.currentTimeMillis();
        // MRLog.error("DOM4J File write time 		:" + (end - start) + "  "
        // + filepath);
    }
    
    public static Document getDocument(URL url) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            return xmlReader.read(url);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public static Document getDocument(InputSource is) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            return xmlReader.read(is);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public static Document getDocument(InputStream is) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            return xmlReader.read(is);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public static Document getDocument(InputStream is, String systemid) throws Exception {
        try {
            SAXReader xmlReader = new SAXReader();
            return xmlReader.read(is, systemid);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    // //////////////////////// document //////////////////////////
    /*    public static Element addElement(String name) {
            return Dom4jUtils.getDocument().addElement(name);
        }
        */

    public static Element addElement(String name) {
        if (name != null) {
            return DocumentHelper.createElement(name);
        }
        return null;
    }
    
    public static Element addElement(Document doc, String name) {
        return doc.addElement(name);
    }
    
    public static Element addElement(Document doc, Element node) {
        doc.add(node);
        return node;
    }
    
    public static Element addElement(Document doc, String name, String value) {
        if (doc == null || name == null) {
            return null;
        }
        if (value != null) {
            return doc.addElement(name).addText(value);
        }
        return doc.addElement(name);
    }
    
    public static Element addElement(Document doc, QName qname) {
        return doc.addElement(qname);
    }
    
    public static Element addElement(Document doc, QName qname, String value) {
        if (doc == null || qname == null) {
            return null;
        }
        if (value != null) {
            return doc.addElement(qname).addText(value);
        }
        return doc.addElement(qname);
    }
    
    // //////////////////////// element //////////////////////////
    
    public static Element addElement(Element node, String name) {
        return node.addElement(name);
    }
    
    public static Element addElement(Element node, Element child) {
        node.add(child);
        return child;
    }
    
    public static Element addElement(Element node, String name, String value) {
        if (node == null || name == null) {
            return null;
        }
        if (value != null) {
            return node.addElement(name).addText(value);
        }
        return node.addElement(name);
    }
    
    public static Element addElement(Element node, QName qname) {
        return node.addElement(qname);
    }
    
    public static Element addElement(Element node, QName qname, String value) {
        if (node == null || qname == null) {
            return null;
        }
        if (value != null) {
            return node.addElement(qname).addText(value);
        }
        return node.addElement(qname);
    }
    
    // //////////////////////// Attribute //////////////////////////
    public static void removeAttribute(Element node, String name) {
        if (node == null && name == null) {
            return;
        }
        try {
            node.addAttribute(name, null);
        } catch (Exception ex) {}
    }
    
    public static Element addAttribute(Element node, String name, String value) {
        if (node == null || name == null) {
            return null;
        }
        if (value == null) {
            value = "";
        }
        return node.addAttribute(name, value);
    }
    
    public static Element addAttributeNonEmpty(Element node, String name, String value) {
        if (node == null && name == null) {
            return null;
        }
        if (value == null || value.trim().equals("")) {
            return null;
        }
        return node.addAttribute(name, value);
    }
    
    public static Element addElementNonEmpty(Element node, String name, String value) {
        if (node == null && name == null) {
            return null;
        }
        if (value == null || value.trim().equals("")) {
            return null;
        }
        return Dom4jUtils.addElement(node, name, value);
    }
    
    public static String getAttributeValue(Element node, String name, String sdefault) {
        if (node == null || name == null) {
            return sdefault;
        }
        String value = node.attributeValue(name);
        if (value != null) {
            return value;
        }
        return sdefault;
    }
    
    public static Element addAttribute(Element node, QName qname, String value) {
        if (node == null || qname == null) {
            return null;
        }
        if (value == null) {
            value = "";
        }
        return node.addAttribute(qname, value);
    }
    
    public static String getAttributeValue(Element node, QName qname, String sdefault) {
        if (node == null || qname == null) {
            return null;
        }
        String value = node.attributeValue(qname);
        if (value != null) {
            return value;
        }
        return sdefault;
    }
    
    public static List<?> getAttributeList(Element parent) {
        if (parent != null) {
            return parent.attributes();
            
        }
        return null;
    }
    
    // //////////////////////// element //////////////////////////
    
    public static Node node(Document document, String name) {
        if (document != null) {
            return document.selectSingleNode(name);
        }
        return null;
    }
    
    public static Element element(Element node, String name) {
        if (node != null) {
            return node.element(name);
        }
        return null;
    }
    
    public static Element getFirstChild(Element node) {
        if (node != null) {
            // MRLog.debug( node.getName()+ " getFirstChild " );
            List<?> childList = node.elements();
            for (Iterator<?> i = childList.iterator(); i.hasNext();) {
                Element el = (Element) i.next();
                return el;
            }
        }
        return null;
    }
    
    public static int getChildCount(Element node) {
        if (node != null) {
            List<?> childList = node.elements();
            return childList.size();
        }
        return 0;
    }
    
    public static Element elementXpath(Document document, String xpathExp) {
        if (document == null) {
            return null;
        }
        XPath xpath = document.createXPath(xpathExp);
        Node node2 = xpath.selectSingleNode(document);
        try {
            if (node2 != null) {
                if (node2 instanceof Element) {
                    return (Element) node2;
                }
            }
        } catch (Exception ex) {}
        return null;
    }
    
    public static Element elementXpath(Element node, String xpathExp) {
        if (node == null) {
            return null;
        }
        return Dom4jUtils.elementXpath((Node) node, xpathExp);
    }
    
    public static Element elementXpath(Node node, String xpathExp) {
        if (node == null) {
            return null;
        }
        XPath xpath = node.createXPath(xpathExp);
        Node node2 = xpath.selectSingleNode(node);
        try {
            if (node2 != null) {
                if (node2 instanceof Element) {
                    return (Element) node2;
                }
            }
        } catch (Exception ex) {}
        return null;
    }
    
    public static Element getElementXpath(Document document, String xpathExp) {
        return Dom4jUtils.elementXpath(document, xpathExp);
    }
    
    public static Element getElementXpath(Element node, String xpathExp) {
        return Dom4jUtils.elementXpath(node, xpathExp);
    }
    public static void replaceElement(Element node, String xpathExp, Element newEL) {
        if(node !=null && newEL!=null){
            Element EL = Dom4jUtils.elementXpath(node, xpathExp);
            if(EL !=null){
                Element oldEL = EL.element(newEL.getName());
                if(oldEL !=null){
                    oldEL.detach();
                }
                Dom4jUtils.addElement(EL,(Element)newEL.clone());
            }
        }
    }
    public static void replaceChildElement(Element node,  Element newEL) {
        if(node !=null && newEL!=null){
            Element oldEL = node.element(newEL.getName());
            if(oldEL !=null){
                oldEL.detach();
            }
            Dom4jUtils.addElement(node,(Element)newEL.clone());
        }
    }
    // //////////////////////// List //////////////////////////
    public static List<?> getList(Document doc) {
        if (doc == null) {
            return new ArrayList<Element>();
        }
        return doc.getRootElement().elements();
    }
    
    public static List<?> getList(Document doc, String childName) {
        if (doc == null || doc.getRootElement() == null) {
            return new ArrayList<Element>();
        }
        // MRLog.debug( " getList2  " + childName );
        return doc.getRootElement().elements(childName);
    }
    
    public static List<?> getList(Element node) {
        if (node == null) {
            return new ArrayList<Element>();
        }
        return node.elements();
    }
    
    public static List<?> getList(Element node, String childName) {
        if (node == null) {
            return new ArrayList<Element>();
        }
        // MRLog.debug( node.getName()+ " getList " + childName );
        return node.elements(childName);
    }
    
    // //////////////////////// List xpath //////////////////////////
    public static List<?> listXpath(Document doc, String xpathExp) {
        if (doc == null) {
            return new ArrayList<Element>();
        }
        XPath xpath = doc.createXPath(xpathExp);
        return xpath.selectNodes(doc);
    }
    
    public static List<?> listXpath(Element node, String xpathExp) {
        if (node == null) {
            return new ArrayList<Element>();
        }
        XPath xpath = node.createXPath(xpathExp);
        return xpath.selectNodes(node);
    }
    
    public static List<?> listXpath(Node node, String xpathExp) {
        if (node == null) {
            return new ArrayList<Element>();
        }
        XPath xpath = node.createXPath(xpathExp);
        return xpath.selectNodes(node);
    }
    
    // //////////////////////// value //////////////////////////
    public static List<String> values(Element node, String nodeName) {
        List<String> vals = new ArrayList<String>();
        if (node != null) {
            //MRLog.info(node.asXML() +"");
            List<?> list = node.elements(nodeName);
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                String val = el.getText();
                vals.add(val);
            }
        }
        return vals;
    }
    public static void replaceValueXpath(Element node, String xpath, String value) {
        if (node != null) {
            Element el = getElementXpath(node, xpath);
            replaceValue(el, xpath, value);
        }
    }
    public static void replaceValue(Element node, String xpath, String value) {
        if (node != null) {
            //todo: if the text is appended to existing node than
            node.setText(value);
        }
    }    
    public static List<String> valuesXpath(Element node, String nodeName) {
        List<String> vals = new ArrayList<String>();
        if (node != null) {
            //MRLog.info(node.asXML() +"");
            List<?> list = node.selectNodes(nodeName);
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                String val = el.getText();
                vals.add(val);
            }
        }
        return vals;
    }
    public static String value(Element node, String nodeName, String sdefault) {
        if (node != null) {
            //MRLog.info(node.asXML());
            String val = node.elementText(nodeName);
            if (val != null) {
                return val.trim();
            }
        }
        return sdefault;
    }
    
    public static String value(Element node, String sdefault) {
        if (node != null) {
            String val = node.getText();
            if (val != null) {
                return val.trim();
            }
        }
        return sdefault;
    }
    public static void setValueCdata(Element node, String value) {
        if (node != null) {
             //CDATA val = new CDATA(value);
        	node.add(DocumentHelper.createCDATA(value));
        }
        
    }
    public static String valueXpath(Document document, String xpathExp, String sdefault) {
        if (document != null) {
            String val = document.valueOf(xpathExp);
            if (val != null) {
                return val;
            }
        }
        return sdefault;
    }
    
    public static String getValueXpath(Element parent, String xpath, String sdefault) throws Exception {
        if (parent != null) {
            Element node = null;
            try {
                node = (Element) parent.selectSingleNode(xpath);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            if (node != null) {
                return Dom4jUtils.value(node, sdefault);
            }
        }
        return sdefault;
    }
    
    public static String valueXpath(Element node, String xpathExp, String sdefault) {
        return Dom4jUtils.valueXpath((Node) node, xpathExp, sdefault);
    }
    
    public static String valueXpath(Node node, String xpathExp, String sdefault) {
        if (node != null) {
            String val = node.valueOf(xpathExp);
            if (val != null) {
                return val;
            }
        }
        return sdefault;
    }
    public static Document loadXMLResource(String xmlFilePath) throws Exception {
    	return loadXMLResource(xmlFilePath, Dom4jUtils.class);
    }
    public static Document loadXMLResource(String xmlFilePath, Class<?> cls) throws Exception {
        URL url = null;
        Document doc = null;
        try {
            url = cls.getResource(xmlFilePath);
        } catch (Exception ex) {
            throw new Exception("XML Resource not found " + xmlFilePath);
        }
        if (url == null) {
            throw new Exception("XML Resource not found " + xmlFilePath);
        }
        try {
            doc = Dom4jUtils.getDocument(url);
        } catch (Exception ex) {
            throw new Exception("XML Resource not found or invalid." + xmlFilePath);
        }
        return doc;
        
    }
    
    public static Document loadXMLFile(String xmlFilePath) throws Exception {
        File file = null;
        Document doc = null;
        // File file1 = new File("temp");
        // MRLog.println( MRLog.DEBUG,"" + file1.getCanonicalPath());
        file = new File(xmlFilePath);
        if (!file.exists()) {
            throw new Exception("XML File not found " + xmlFilePath);
        }
        try {
            doc = Dom4jUtils.getDocument(file);
        } catch (Exception ex) {
            MRLog.error(ex);
            throw new Exception("XML File not found  or invalid." + xmlFilePath);
        }
        return doc;
    }
    
    /*
     * public static Document loadXML(String xmlFilePath ) throws Exception { Document doc = null; xmlFilePath =
     * xmlFilePath.replace('\\', '/'); //TODO: wrong assumption for Linux/Unix if (xmlFilePath.startsWith("/") ) { doc =
     * loadXMLResource(xmlFilePath); } else { doc = loadXMLFile(xmlFilePath); } return doc; }
     */
    public static Document loadXML(String xmlFilePath, String loadFrom) throws Exception {
        Document doc = null;
        if (loadFrom.equalsIgnoreCase("JAR")) {
            doc = Dom4jUtils.loadXMLResource(xmlFilePath);
        } else if (loadFrom.equalsIgnoreCase("FILE")) {
            doc = Dom4jUtils.loadXMLFile(xmlFilePath);
        }
        return doc;
    }
    
    public static void saveDocument(String filePath, Document doc) throws Exception {
        if (doc != null) {
            BufferedWriter out = new BufferedWriter(new FileWriter(filePath));
            out.write(Dom4jUtils.documentToString(doc));
            out.close();
        }
    }
    
    public static Object selectSingleNodeXpath(Element parent, String xpath) throws Exception {
        if (parent != null) {
            try {
                return parent.selectSingleNode(xpath);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return null;
    }
    
    public static Object selectSingleNodeXpath(Document doc, String xpath) throws Exception {
        if (doc != null) {
            try {
                return doc.selectSingleNode(xpath);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return null;
    }

    public static Element getDescendentWithoutNamespace(Element parent, String name) throws Exception {
        if (parent==null) {
            return null;
        }
        if (name.equals(parent.getName())) {
            return parent;
        }
        List<?> children = parent.elements();
        Iterator<?> i = children.iterator();
        Element element = null;
        while (i.hasNext()) {
            element = getDescendentWithoutNamespace((Element) i.next(), name);
            if (element != null) {
                break;
            }
        }
        return element;
    }

}
