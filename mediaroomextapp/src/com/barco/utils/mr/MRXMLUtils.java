package com.barco.utils.mr;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.StringReader;
import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.validation.SchemaFactory;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.Node;
import org.dom4j.QName;
import org.dom4j.XPath;
import org.dom4j.io.SAXReader;
import org.dom4j.util.NodeComparator;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;


public final class MRXMLUtils {
    public static final String SCHEMA_NAMESPACE_URI = "http://www.w3.org/2001/XMLSchema";
    private static String SERVER_VERSION = null;
    private static String SERVER_VERSION_XML = "/com/ipvs/xsd/objectXml/service.version";
    
    private static class DOM4JErrorHandler implements ErrorHandler {

		@Override
		public void error(SAXParseException exception) throws SAXException {
			throw exception; 
		}

		@Override
		public void fatalError(SAXParseException exception) throws SAXException {
			throw exception; 
			
		}

		@Override
		public void warning(SAXParseException exception) throws SAXException {
			throw exception; 
			
		}
    	
    }
       ///// SAXReader pool ///////////////
    private static BlockingQueue<SAXReader> xmlReaders = new LinkedBlockingQueue<SAXReader>();
    static {
        // Initialize the pool of sax readers
        /*for (int i = 0; i < 10; i++) {
            SAXReader xmlReader = new SAXReader();
            // xmlReader.setEncoding("UTF-8");
            xmlReaders.add(xmlReader);
        }*/
    }

    public static void checkInSAXReader(SAXReader xmlReader) {
        if(xmlReader !=null){
            //xmlReaders.add(xmlReader);
            xmlReader=null;
        }
    }

    public static SAXReader checkOutSAXReader() throws InterruptedException {
        SAXReader xmlReader =  new SAXReader();//xmlReaders.take();
        return xmlReader;
    }

    ///// SAXReader pool ///////////////  
 
    public static Document getDocument() throws Exception {
        return DocumentHelper.createDocument();
    }
    public static synchronized Document getDocument(URL url) throws Exception {
        SAXReader xmlReader = null;
        try {
            xmlReader = checkOutSAXReader();
            return xmlReader.read(url);
        } finally {
            checkInSAXReader(xmlReader);
        }        
         
    }

    public static synchronized Document getDocument(File f) throws Exception {
        SAXReader xmlReader = null;
        try {
            xmlReader = checkOutSAXReader();
            return xmlReader.read(f);
        } finally {
            checkInSAXReader(xmlReader);
        }     
    }
 

    /*
    public static  Document getDocument(String xml, String xsdURL) throws Exception {
        URL url = null;
        try {
            url = MRDB.class.getResource(xsdURL);
        } catch (Exception ex) {
            throw new MRException(MRExceptionCodes.MRSCHEMA_XML_NOT_FOUND, generateErrorXML("xsdFilePath", xsdURL));
        }       
        try {
            SAXReader reader = new SAXReader(true);
            reader.setFeature("http://apache.org/xml/features/validation/schema", true);
            reader.setProperty("http://java.sun.com/xml/jaxp/properties/schemaLanguage", "http://www.w3.org/2001/XMLSchema");
            reader.setProperty("http://java.sun.com/xml/jaxp/properties/schemaSource", new File(url.getFile()));
            //reader.setErrorHandler(new ErrorHandlerImpl());
            InputSource source = new InputSource(new StringReader(xml));
            source.setEncoding("UTF-8");
            return reader.read(source);
        } catch (Exception e) {
            throw new MRException(MRExceptionCodes.MRSCHEMA_XML_NOT_VALID, generateErrorXML("mrschema", "validation failed" + e.toString()));
        }         
    }
*/

    public static synchronized Document getDocument(String xml) throws Exception {
    	return DocumentHelper.parseText(xml);
    }
    
    /*
    public static synchronized void validateXML(String xmlString, String xsdFilePath) throws Exception {
      try {
    		URL url = null;
            try {
                url = MRDB.class.getResource(xsdFilePath);
            } catch (Exception ex) {
                throw new Exception(xsdFilePath) ;
            }
            SAXParserFactory factory = SAXParserFactory.newInstance();

            SchemaFactory schemaFactory = 
                SchemaFactory.newInstance("http://www.w3.org/2001/XMLSchema");

            factory.setSchema(schemaFactory.newSchema(url));

            SAXParser parser = factory.newSAXParser();

            SAXReader reader = new SAXReader(parser.getXMLReader());
            reader.setValidation(false);
            reader.setErrorHandler(new DOM4JErrorHandler());
            reader.read(new StringReader(xmlString) );

		} catch (Exception e) {
            throw new MRException(MRExceptionCodes.MRSCHEMA_XML_NOT_VALID, generateErrorXML("mrschema", "validation failed" + e.toString()));
		}    	
    	
    }
    */
    
    //////////////// save/load document ////////////////
    public static void saveDocument(String filePath, Document doc) throws Exception {
        if (doc != null) {
            BufferedWriter out = new BufferedWriter(new FileWriter(filePath));
            out.write(documentToString(doc));
            out.close();
        }
    }

    public static Document loadXMLResource(String xmlFilePath, MRLog log) throws Exception {
        URL url = null;
        Document doc = null;
        /*
        if (xmlFilePath.indexOf("_BASE_") != -1) {
            xmlFilePath = CreateRequestXmlUtil.resolveOldRequestXMLName(xmlFilePath).replace('\\', '/');
        }
        */

        try {
            url = MRXMLUtils.class.getResource(xmlFilePath);
        } catch (Exception ex) {
            // TODO: create Exception Variable.
            log.addLog(MRLog.INFO, MRLog.OK, "loadXMLResource url failed:" + xmlFilePath + " " + ex.toString());
            throw new MRException(MRExceptionCodes.XML_NOT_FOUND, MRXMLUtils.generateErrorXML("xsdFilePath", xmlFilePath));
        }

        try {
            doc = MRXMLUtils.getDocument(url);
        } catch (Exception ex) {
            // TODO: create Exception Variable.
            log.addLog(MRLog.INFO, MRLog.OK, "loadXMLResource doc failed:" + xmlFilePath + " " + ex.toString());
            throw new MRException(MRExceptionCodes.XML_NOT_VALID, MRXMLUtils.generateErrorXML("xsdFilePath", xmlFilePath));
        }
        //only verify
        verifyServiceVersion(doc, log, xmlFilePath);
        //addServiceVersion(doc,log, xmlFilePath);

        return doc;
    }

    public static Document loadXMLFile(String xmlFilePath, MRLog log) throws Exception {
        File file = null;
        Document doc = null;
        file = new File(xmlFilePath);

        if (!file.exists()) {
            log(MRLog.INFO, MRLog.OK, "loadXMLFile file " + xmlFilePath + " does not exis ", log);
            throw new MRException(MRExceptionCodes.XML_NOT_FOUND, MRXMLUtils.generateErrorXML("xsdFilePath", xmlFilePath));
        }

        try {
            doc = MRXMLUtils.getDocument(file);
        } catch (Exception ex) {
            log(MRLog.INFO, MRLog.OK, "loadXMLFile doc  " + xmlFilePath + " failed:" + ex.toString(), log);
            throw new MRException(MRExceptionCodes.XML_NOT_VALID, MRXMLUtils.generateErrorXML("xsdFilePath", xmlFilePath));
        }
        //only verify
        verifyServiceVersion(doc, log, xmlFilePath);
        //addServiceVersion(doc,log, xmlFilePath);
        return doc;
    }

    public static Document loadXML(String xmlFilePath, MRLog log) throws Exception {
        Document doc = null;
        xmlFilePath = xmlFilePath.replace('\\', '/');
        //File file = new File(xmlFilePath);
        //if (!file.exists()) { //xmlFilePath.startsWith("/")) {
        if (xmlFilePath.startsWith("/")) {
            doc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        } else {
            doc = MRXMLUtils.loadXMLFile(xmlFilePath, log);
        }

        return doc;
    }

    public static Document loadXML(String xmlFilePath, String loadFrom, MRLog log) throws Exception {
        Document doc = null;
        // <ValidationXsd loadFrom="JAR"
        // file="/src/com/ipvs/xsd/mediaroom.assetadminservice.datamodel.Device.xsd"
        // />
        if (loadFrom.equalsIgnoreCase("JAR")) {
            doc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        } else if (loadFrom.equalsIgnoreCase("FILE")) {
            doc = MRXMLUtils.loadXMLFile(xmlFilePath, log);
        }

        return doc;
    }

    //////////////////// element document to string and vice versa/////////
    
    public static Document stringToDocument(String xml) throws Exception {
        if ((xml == null) || xml.trim().equals("")) {
            //TODO: add to messages.properties with error code.
            //more errors to be raised in this class, there are getDocumentBuilder().build exceptions.
            throw new Exception("Error in parse, Empty xml-'" + xml + "'.");
        }

        if (!xml.trim().startsWith("<")) {
            throw new Exception("Error in parse, Invalid xml-'" + xml + "'.");
        }

        /*StringReader reader = new StringReader(xml);
        InputSource source = new InputSource(reader);

        return getDocumentBuilder().build(source);*/
        SAXReader xmlReader = null;
        try {
            xmlReader = checkOutSAXReader();
            return xmlReader.read(new StringReader(xml));
        }catch(Exception ex){
            //MRLog.println("stringToDocument error:" +xml);
            throw ex;
        } finally {
            checkInSAXReader(xmlReader);
        }        
    }

    // document to string
    public static String documentToString(Document doc) throws Exception {
        /*StringWriter ws = new StringWriter();

        // Format format = Format.getRawFormat();
        // Format.getCompactFormat();
        Format format = Format.getPrettyFormat();
        XMLOutputter outp = new XMLOutputter(format);
        // outp.setTextTrim(true);
        // outp.setIndent(" ");
        // outp.setNewlines(true);
        outp.output(doc, ws); // System.out);

        return ws.toString();*/
        if (doc == null) {
            return null;
        }
        return doc.asXML();
    }

    public static String documentToStringOmitDeclaration(Document doc) throws Exception {
    	return doc.getRootElement().asXML();
    }

    public static String elementToString(Element parent) throws Exception {
        if (parent != null) {
            /*StringWriter ws = new StringWriter();
            Format format = Format.getRawFormat();
            // Format.getPrettyFormat();Format.getCompactFormat();
            XMLOutputter outp = new XMLOutputter(format);
            // outp.setTextTrim(true);
            // outp.setIndent(" ");
            // outp.setNewlines(true);
            outp.output(parent, ws); // System.out);
            return ws.toString(); */
            return parent.asXML();            
        } else {
            return null;
        }
    }

    public static Document elementToDocument(Element parent) throws Exception {
        /*Element newRoot = (Element) parent.clone();
        Document doc = getDocument();
        doc.addContent(newRoot);
        return doc;*/
        Element newRoot = parent.createCopy();
        Document doc = getDocument();
        doc.add(newRoot);
        return doc;        
    }

    public static Element stringToElement(String xml) throws Exception {
        Document doc = stringToDocument(xml);

        if (doc != null) {
            return (Element) doc.getRootElement().clone();
        }

        return null;
    }

    public static Element tagToElement(String tag) throws Exception {
        Document tagdoc = MRXMLUtils.stringToDocument(tag);
        return (Element) tagdoc.getRootElement().clone();
    }

    //////////////////// element document to string and vice versa/////////

    ////////////////////////// selectNodes  selectSingleNode ///////////////////////
    /*
    the list of selected items, which may be of
    types: Element, Attribute, Text, CDATA, Comment, ProcessingInstruction, Boolean, Double, or String.
    */

    /*
     the list of selected items, which may be of
      types: Element, Attribute, Text, CDATA, Comment, ProcessingInstruction, Boolean, Double, or String.
     */
    //   public static List<?> selectNodesXpath(Element parent, String xpath, Namespace ns)
    //       throws Exception {
    //       XPath x = XPath.newInstance(xpath);
    //       x.addNamespace(ns);
    //
    //       return x.selectNodes(parent);
    //   }
    /*
    the first selected item, which may be of
    types: Element, Attribute, Text, CDATA, Comment, ProcessingInstruction, Boolean, Double, String, or null if no item was selected.
    */
    public static List<?> selectNodesXpath(Element parent, String xpath) throws Exception {
        XPath xpathObj = DocumentHelper.createXPath(xpath);
        return xpathObj.selectNodes(parent);
    }
    
    public static List<?> selectNodesXpath(Document doc, String xpath) throws Exception {
        XPath xpathObj = DocumentHelper.createXPath(xpath);
        return xpathObj.selectNodes(doc);
    }
    
    public static Node selectSingleNodeXpath(Element parent, String xpath) throws Exception {
        XPath xpathObj = DocumentHelper.createXPath(xpath);
        Node node= xpathObj.selectSingleNode(parent);
        //if(node==null){
        //    System.out.println("1 selectSingleNodeXpath parentNUll ?"+ (parent==null) + "'" + xpath  +"'" + MRXMLUtils.elementToString(parent).replace('\n', ' ') );
        //}
        return node;
    }

    public static Node selectSingleNodeXpath(Document doc, String xpath) throws Exception {
        XPath xpathObj = DocumentHelper.createXPath(xpath);
        Node node= xpathObj.selectSingleNode(doc);
        //if(node==null){
         //   System.out.println("2 selectSingleNodeXpath docNUll ?"+ (doc==null) + "'" + xpath  +"'" + MRXMLUtils.documentToString(doc).replace('\n', ' ') );
        //}
        return node;
    }

    /*
    the first selected item, which may be of
    types: Element, Attribute, Text, CDATA, Comment, ProcessingInstruction, Boolean, Double, String, or null if no item was selected.
    */
    //   public static Object selectSingleNodeXpath(Element parent, String xpath, Namespace ns)
    //       throws Exception {
    //       XPath x = XPath.newInstance(xpath);
    //       x.addNamespace(ns);
    //
    //       return x.selectSingleNode(parent);
    //   }
    ////////////////////////// selectNodes  selectSingleNode ///////////////////////
    /////////////////////// get //////////////////////////
    // GET Read Element / attributes
    public static Element getRootElement(Document doc) throws Exception {
        return doc.getRootElement();
    }

    public static String getAttributeValue(Element root, String parentElementName, String attibutename, String defaultValue) throws Exception {
        Element parent = getElement(root, parentElementName);

        return getAttributeValue(parent, attibutename, defaultValue);
    }

    public static String getAttributeValue(Element parent, String attibutename) {
        if (parent != null) {
            Attribute attrib = parent.attribute(attibutename);
            if (attrib != null) {
                return attrib.getValue();
            }
        }
        return null;
    }

	public static String getAttributeValue(Element parent, String attibutename, String defaultValue) {
        if (parent != null) {
            Attribute attrib = parent.attribute(attibutename);

            if (attrib != null) {
                return attrib.getValue();
            }
        }

        return defaultValue;
    }
	
	  public static String getAttributeValueWithAlternate(Element parent, String attribute1, String attribute2, String defaultValue)
	    {
	    	String attrValue = getAttributeValue(parent, attribute1);
	    	if(attrValue != null) {
	    		return attrValue;
	    	}
	    		
	    	return getAttributeValue(parent, attribute2, defaultValue);
	    }

    public static String getAttributeValueXpath(Element root, String parentElementXpath, String attibutename, String defaultValue) throws Exception {
        Element parent = getElementXpath(root, parentElementXpath);

        return getAttributeValue(parent, attibutename, defaultValue);
    }

    public static String getAttributeValueXpath(Document root, String parentElementXpath, String attibutename, String defaultValue) throws Exception {
        Element parent = getElementXpath(root, parentElementXpath);

        return getAttributeValue(parent, attibutename, defaultValue);
    }

    public static String getAttributeValueXpath(Element parent, String attibuteXpath, String defaultValue) throws Exception {
        String value=null;
        if (parent != null) {
            Node obj = selectSingleNodeXpath(parent, attibuteXpath);    
            if (obj != null) {
                if (obj instanceof Attribute) {
                    Attribute attr = (Attribute) obj;
                    value= attr.getValue();
                }else if (obj instanceof Element) {
                    Element el = (Element) obj;
                    value= el.getStringValue();
                }else{
                    value= obj.toString();
                }
            }
        }
        if(value==null ){
            return defaultValue;
        }
        return value;
    }

    public static Attribute getAttribute(Element parent, String attibutename) {
        if (parent != null) {
            Attribute attrib = parent.attribute(attibutename);

            return attrib;
        }

        return null;
    }
    
    public static List<?> getAttributeList(Element parent) {
        if (parent != null) {
            return parent.attributes();
        }

        return null;
    }

    // Element value
    public static String getValue(Document doc, String name, String defaultValue) throws Exception {
        return getValue(doc.getRootElement(), name, defaultValue);
    }

    public static String getValue(Element parent, String name, String defaultValue)  {
        if (parent != null) {
            Element el = parent.element(name);
            return getValue2(el, defaultValue);
        }
        return null;
    }

    public static String getValue(Element el){
        if (el != null) {
            return el.getText();
        }
        return null;
    }

    public static String getValue2(Element el, String defaultValue) {
        if (el != null) {
            return el.getText();
        }
        return defaultValue;
    }

    public static Element getElement(Document doc, String name) throws Exception {
        return getElement(doc.getRootElement(), name);
    }

    public static Element getElement(Element parent, String name) throws Exception {
        if (parent != null) {
            return parent.element(name);
        }
        return null;
    }

    public static List<?> getList(Document doc, String name)  {
        return getList(doc.getRootElement(), name);
    }

    public static List<?> getList(Element parent, String name) {
        if (parent != null) {
            return parent.elements(name);
        }
        return new ArrayList<Element>();
    }

    public static List<?> getList(Element parent){
        if (parent != null) {
            return parent.elements();
        }
        return new ArrayList<Element>();
    }
   

    //    public static List<?> getList(Element parent, String name, Namespace namesapce)
    //        throws Exception {
    //        if (parent != null) {
    //            return parent.elements(name, namesapce);
    //        }
    //
    //        return null;
    //    }

    public static boolean hasChilds(Element parent) throws Exception {
        if (parent != null) {
            return getList(parent).isEmpty();
        }
        return false;
    }

    public static Element getFirstChildXpath(Element actionEL, String xpath) throws Exception {
        Element parent = MRXMLUtils.getElementXpath(actionEL, xpath);
        Element child = null;
        if (parent != null) {
            List<?> childList = MRXMLUtils.getList(parent);
            if (childList.size() > 0) {
                child = (Element) childList.get(0);
            }
        }
        return child;
    }

    public static Element getFirstChild(Element parent) throws Exception {
        Element child = null;
        if (parent != null) {
            List<?> childList = MRXMLUtils.getList(parent);
            if (childList.size() > 0) {
                child = (Element) childList.get(0);
            }
        }
        return child;
    }

    // XPath
    public static Element getElementXpath(Document doc, String xpath) throws Exception {
        //return (Element)doc.selectSingleNode(xpath);
        return (Element)selectSingleNodeXpath(doc, xpath);
    }

    public static boolean applyXPath(Document doc, String xpath) throws Exception {
        /*XPath x = XPath.newInstance(xpath);
        return (Boolean)doc.selectSingleNode(xpath);*/
        /*Node node = selectSingleNodeXpath(doc, xpath);
        if(node !=null ) {
            String value= node.getStringValue();
            if(value.equalsIgnoreCase("true")){
                new Boolean(false);
            }
        }
        return new Boolean(false);*/
    	
    	XPath xpathObj = DocumentHelper.createXPath(xpath);
        return (Boolean)xpathObj.evaluate(doc);
    }

    // Get Document By XPath
    public static Document getDocumentXpath(Document doc, String xpath) throws Exception {
        Element objEL =(Element)selectSingleNodeXpath(doc, xpath);
        if (objEL == null) {
            return null;
        }
        return MRXMLUtils.elementToDocument(objEL);
    }

    public static Element getElementXpath(Element parent, String xpath) throws Exception {
        //return (Element) parent.selectSingleNode(xpath);
        return  (Element)selectSingleNodeXpath(parent, xpath);
    }

    public static String getValueXpath(Document doc, String xpath, String defaultValue) throws Exception {
        Element el = getElementXpath(doc, xpath);
        return getValue2(el, defaultValue);
    }

    public static String getValueXpath(Element parent, String xpath, String defaultValue) throws Exception {
        //todo: found errors during admin setup. needs to find why xpath are invalid.
        try {
            Element el = getElementXpath(parent, xpath);
            /*
                    if (parent != null) {
                            XPath x = XPath.newInstance(attibuteXpath);
                            Object obj = x.selectSingleNode(parent);
                            if( obj != null ) {
                                    if (obj instanceof Attribute) {
                                            Attribute attr = (Attribute) obj;
                                            return attr.getValue();
                                    }
                                    if (obj instanceof Element) {
                                            Element el = (Element) obj;
                                            return el.getValue();
                                    }
                                    //TODO
                                    return obj.toString();
            
                            }
                    }
                */
            return getValue2(el, defaultValue);
        } catch (Exception ex) {
            //System.out.print(xpath + " xpath " + elementToString(parent));
            throw ex;
        }
    }

    public static List<?> getListXpath(Document doc, String xpath) throws Exception {
        return  selectNodesXpath(doc, xpath);
    }

    public static List<?> getListXpath(Element parent, String xpath) throws Exception {
        return  selectNodesXpath(parent, xpath);
    }

    public static String[] getValuesAsArr(Element parent, String elementName) throws Exception {
        List<String> vals = getValues(parent, elementName);
        String[] ret = null;
        vals.toArray(ret);
        return ret;
    }

    public static String[] getValuesXpathAsArr(Element parent, String xpath) throws Exception {
        List<String> vals = getValuesXpath(parent, xpath);
        String[] ret = null;
        if (vals != null && vals.size() > 0) {
            ret = new String[vals.size()];
            vals.toArray(ret);
        }
        return ret;
    }

    public static List<String> getValues(Element parent, String elementName) throws Exception {
        List<String> vals = new Vector<String>();
        List<?> list = getList(parent, elementName);
        if (list != null && list.size() > 0) {
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                String val = el.getText();
                vals.add(val);
            }
        }
        return vals;
    }

    public static List<String> getValuesXpath(Element parent, String xpath) throws Exception {
        List<String> vals = new Vector<String>();
        List<?> list = getListXpath(parent, xpath);
        if (list != null && list.size() > 0) {
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Node el = (Node) iterator.next();
                String val = el.getText();
                vals.add(val);
            }
        }
        return vals;
    }
    
    public static List<String> getValuesXpath(Document document, String xpath) throws Exception {
        List<String> vals = new ArrayList<String>();
        List<?> list = getListXpath(document, xpath);
        if(list==null || list.isEmpty()) {
        	return null;
        }
        Node node= null;
        for(Object object: list) {
        	node = (Node) object;
        	vals.add(node.getText());
        }
        
        return vals;
    }

    //////////////////////// get ///////////////////

    //////////////////////// // add Element and Attributes ////////////////////////
    public static Element addAttribute(Element parent, String name, String value)  {
        parent.addAttribute(name, value);
        return parent;
    }

    //    public static Element addAttribute(Element parent, String name, String value, Namespace namespace)
    //        throws Exception {
    //        parent.addAttribute(name, value, namespace);
    //        return parent;
    //    }
    public static Element newElement(String name) {
        return DocumentHelper.createElement(name);
    }
    public static Element addElement(String name) throws Exception {
        return newElement(name);
    }

    public static Element addElement(String name, String value) throws Exception {
        Element e = newElement(name);
        if (value != null) {
            e.setText(value);
        }

        return e;
    }

    /*public static Element addElementFromXml(String name, String value) throws Exception {
        Element e = new Element(name);
        return addElementFromXml(e, value);
    }

    public static Element addElementFromXml(Element parent, String newChildXml)
        throws Exception {
        Element newChildXmlRoot = addElementFromXml(newChildXml);
        if (parent != null) {
            parent.addContent(newChildXmlRoot);
        }
        return newChildXmlRoot;
    }
    */
    public static Element addElementFromXml(String newChildXml) throws Exception {
        Document newChildXmlDoc = stringToDocument(newChildXml);
        Element newChildXmlRoot = (Element) newChildXmlDoc.getRootElement().clone();
        return newChildXmlRoot;
    }

    public static Element addElement(Document doc, String name) {
        return addElement(doc, name, null);
    }

    public static Element addElement(Element parent, Element child)  {
        parent.add(child);
        return child;
    }

    public static Element addElement(Document doc, String name, String value) {
        Element e = newElement(name);
        if (value != null) {
            e.setText(value);
        }
        doc.add(e);
        return e;
    }

    //    public static Element addElement(String name, String value, Namespace namespace)
    //        throws Exception {
    //        Element e = new Element(name, namespace);
    //        if (value != null) {
    //            e.setText(value);
    //        }
    //        return e;
    //    }

    //    public static Element addElement(Document doc, String name, String value, Namespace namespace)
    //        throws Exception {
    //        Element e = new Element(name, namespace);
    //        if (value != null) {
    //            e.setText(value);
    //        }
    //        doc.addContent(e);
    //        return e;
    //    }

    public static Element addElement(Element parent, String name) {
        return addElement(parent, name, null);
    }

    public static Element addElement(Element parent, String name, String value) {
        Element e = newElement(name);
        if (value != null) {
            e.setText(value);
        }
        parent.add(e);
        return e;
    }

//    public static Element addElement(Element parent, String name, String value, Namespace namespace) throws Exception {
//        Element e = new Element(name, namespace);
//        if (value != null) {
//            e.setText(value);
//        }
//        parent.addContent(e);
//        return e;
//    }
    public static Element addElementXpath(Document parent, String to_child_xpath, Element child) throws Exception {
        Element e = getElementXpath(parent, to_child_xpath);
        if (e != null) {
            return addElement(e, child) ;
        }
        return null;
    }   
    public static Element addElementXpath(Element parent, String to_child_xpath, Element child) throws Exception {
        Element e = getElementXpath(parent, to_child_xpath);
        if (e != null) {
            return addElement(e, child) ;
        }
        return null;
    }    
    public static Element addElementXpath(Document parent, String to_child_xpath, String name, String value) throws Exception {
        Element e = getElementXpath(parent, to_child_xpath);
        if (e != null) {
            return addElement(e, name, value) ;
        }
        return null;
         
    }
    public static Element addElementXpath(Element parent, String to_child_xpath, String name, String value) throws Exception {
        Element e = getElementXpath(parent, to_child_xpath);
        if (e != null) {
            return addElement(e, name, value) ;
        }
        return null;
    }

    public static Element addElement(Document doc, Element newChild) throws Exception {
        doc.add(newChild);
        return newChild;
    }


    //////////////////////// set /////////////////////

    //set attribute value 
    public static void setAttributeValue(Element parent, String attributeName, String attributevalue) throws Exception {
        if ((parent == null) || (attributeName == null) || attributeName.isEmpty()) {
            return;
        }
        parent.addAttribute(attributeName, attributevalue);
    }

    //set attribute value 
    public static void setAttributeValueXPath(Element parent, String xpath, String attributeName, String attributevalue) throws Exception {
        if ((parent == null) || (attributeName == null) || attributeName.isEmpty()) {
            return;
        }
        Element childEl = getElementXpath(parent, xpath);
        setAttributeValue(childEl, attributeName, attributevalue);
    }

    //set attribute value    
    public static void setAttributeValueXPath(Document document, String xpath, String attributeName, String attributevalue) throws Exception {
        if ((document == null) || (attributeName == null) || attributeName.isEmpty()) {
            return;
        }
        Element childEl = getElementXpath(document, xpath);
        setAttributeValue(childEl, attributeName, attributevalue);
    }

    // change add value
    public static Element setValue(Element e, String value) throws Exception {
        if (e != null) { //&& value != null)
            if(value!=null){
                e.setText(value);
            }
        }
        return e;
    }

    public static Element replaceValue(Element parent, String childName, String value) throws Exception {
        if (parent != null) {
            return replaceValue(getElement(parent, childName), value);
        }
        return null;
    }

    public static Element replaceValue(Element e, String value) throws Exception {
        if (e != null) {
            //e.setText(text)   
            //          ContentFilter filter = new ContentFilter(); //ContentFilter.TEXT | ContentFilter.CDATA
            //          //filter.setElementVisible(false);
            //          List content = e.getContent(filter);
            //          
            //          e.removeContent(filter);
            //          e.addContent( new Text(value));

            //TODO: there is problem in this function
            Element parent = e.getParent();
            String elementName = e.getName();
            Node b = e.detach();
            if (b != null) {
                e = newElement(elementName);
                if (value != null) {
                    e.setText(value);
                }
            }
            parent.add(e);
        }
        return e;
    }

    public static Element setValueXpath(Element parent, String childXpath, String value) throws Exception {
        if (parent != null) {
            return setValue(getElementXpath(parent, childXpath), value);
        }
        return null;
    }

    public static Element setValueXpath(Document parent, String childXpath, String value) throws Exception {
        if (parent != null) {
            return setValue(getElementXpath(parent, childXpath), value);
        }
        return null;
    }

    public static Element replaceValueXpath(Element parent, String childXpath, String value) throws Exception {
        if (parent != null) {
            return replaceValue(getElementXpath(parent, childXpath), value);
        }
        return null;
    }

    //////////////////////// // add Element and Attributes ////////////////////////

    //////////////// service version ////////////////////////
    public static void addServiceVersion(Document doc, MRLog log, String log_description) throws Exception {
        if (doc != null && doc.getRootElement() !=null) {
            addServiceVersion(doc.getRootElement(), log, log_description);
        }
    }
    // private static Object synclog= new Object();
    public static void addServiceVersion(Element el, MRLog log, String log_description) throws Exception {
        if (el != null) {
            boolean bapplied = false;
            try {
                if (SERVER_VERSION == null) {
                    getServiceVersion(log);
                }
                try {
                    String serviceVer = el.attributeValue("serviceVer");
                    if (serviceVer != null && serviceVer.trim().equals("")) {
                        el.addAttribute("serviceVer", SERVER_VERSION);
                        bapplied = true;
                        /*if (xpath_serviceVer == null) {
                            xpath_serviceVer = XPath.newInstance("child::*[ @serviceVer and string-length(normalize-space(@serviceVer)) = 0 ]");
                        }*/
                        List<?> list = el.selectNodes( "child::*[ @serviceVer and string-length(normalize-space(@serviceVer)) = 0 ]");
                        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                            try {
                                Element elChild = (Element) iterator.next();
                                elChild.addAttribute("serviceVer", SERVER_VERSION);
                            } catch (Exception ex) {
                                log(MRLog.INFO, MRLog.OK, "Adding Service version failed:" + ex.toString(), log);
                            }
                        }
                    }
                } catch (Exception ex) {
                    log(MRLog.INFO, MRLog.OK, "Adding Service version failed:" + ex.toString(), log);
                    throw new MRException(MRExceptionCodes.SERVICE_VERSION_ADD_ATTRB_FAILED, MRXMLUtils.generateErrorXML("error", ex.toString()));
                }
            } finally {
                //todo:remove.  temp for testing purpose only 
                //String lineSeparator=System.getProperty("line.separator","\n");
                String str = MRXMLUtils.elementToString(el);
                if (str != null && str.length() > 100) {
                    //synchronized (synclog) {
                    if (bapplied) {
                        //log.addInfo("DID_APPLY SERVER_VERSION " + SERVER_VERSION + " " +  MRXMLUtils.elementToString(el).substring(0,99).replace(lineSeparator, " "), MRLog.INFO, MRLog.NOTIFY);
                        // System.out.println("DID_APPLY SERVER_VERSION " + SERVER_VERSION + " " +  el.getName() +" " + log_description);
                    } else {
                        //log.addInfo("DID_NOT_APPLY SERVER_VERSION " + SERVER_VERSION + " " + MRXMLUtils.elementToString(el).substring(0,99).replace(lineSeparator, " "), MRLog.INFO, MRLog.NOTIFY);
                        // System.out.println("DID_NOT_APPLY SERVER_VERSION " + SERVER_VERSION + " " +   el.getName() +" " + log_description);
                    }

                    //}
                }
            }
        }
    }

    public static void verifyServiceVersion(Document doc, MRLog log, String log_description) throws Exception {
        if (doc != null && doc.getRootElement() !=null) {
            verifyServiceVersion(doc.getRootElement(), log, log_description);
        }
    }

    // private static Object synclog= new Object();
    public static void verifyServiceVersion(Element el, MRLog log, String log_description) throws Exception {
        if (el != null) {
            boolean bapplied = true;
            String nodeName = el.getName();
            try {
                if (SERVER_VERSION == null) {
                    getServiceVersion(log);
                }
                try {
                    String serviceVer = el.attributeValue("serviceVer");
                    if (serviceVer == null) {
                        if (nodeName.equals("Request") || nodeName.equals("Response")) {
                            //is handler request/response
                            bapplied = false;
                        } else {
                            //is object xml
                            String nid = el.attributeValue("NID");
                            if (nid != null) {
                                bapplied = false;
                            }
                        }
                    } else if (serviceVer.trim().equals("")) {
                        //serviceVer is there bu empty
                        bapplied = false;
                    } else {
                        //if not null on root...may be empty on child
                        /*if (xpath_serviceVer_verify == null) {
                            xpath_serviceVer_verify = XPath.newInstance("child::*[ @NID or @serviceVer ]");
                        }*/
                        List<?> list = el.selectNodes("child::*[ @NID or @serviceVer ]");
                        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                            try {
                                Element elChild = (Element) iterator.next();
                                serviceVer = elChild.attributeValue("serviceVer");
                                if (serviceVer == null || serviceVer.trim().equals("")) {
                                    bapplied = false;
                                    nodeName = nodeName + ":" + elChild.getName();
                                }
                            } catch (Exception ex) {
                                log(MRLog.INFO, MRLog.OK, "Verifying Service version failed:" + ex.toString(), log);
                            }
                        }
                    }

                } catch (Exception ex) {
                    log(MRLog.INFO, MRLog.OK, "Verifying Service version failed:" + ex.toString(), log);
                    throw new MRException(MRExceptionCodes.SERVICE_VERSION_ADD_ATTRB_FAILED, MRXMLUtils.generateErrorXML("error", ex.toString()));
                }
            } finally {
                if (bapplied) {
                    //log.addInfo("DID_APPLY SERVER_VERSION " + SERVER_VERSION + " " +  MRXMLUtils.elementToString(el).substring(0,99).replace(lineSeparator, " "), MRLog.INFO, MRLog.NOTIFY);
                    //System.out.println("DID_APPLY SERVER_VERSION " + SERVER_VERSION + " " +  nodeName +" " + log_description);
                } else {
                    //log.addInfo("DID_NOT_APPLY SERVER_VERSION " + SERVER_VERSION + " " + MRXMLUtils.elementToString(el).substring(0,99).replace(lineSeparator, " "), MRLog.INFO, MRLog.NOTIFY);
                    //System.out.println("DID_NOT_APPLY SERVER_VERSION " + SERVER_VERSION + " " +   nodeName +" " + log_description);
                }
            }
        }
    }

    public static String getServiceVersion(MRLog log) throws Exception {
        if (SERVER_VERSION == null) {
            URL url = null;
            Document doc = null;
            try {
                url = MRXMLUtils.class.getResource(SERVER_VERSION_XML);
            } catch (Exception ex) {
                log(MRLog.INFO, MRLog.OK, "loadXMLResource url " + SERVER_VERSION_XML + " failed:" + ex.toString(), log);
                throw new MRException(MRExceptionCodes.SERVICE_VERSION_XML_NOT_FOUND, MRXMLUtils.generateErrorXML("error", SERVER_VERSION_XML));
            }

            try {
                doc = MRXMLUtils.getDocument(url);
            } catch (Exception ex) {
                log(MRLog.INFO, MRLog.OK, "loadXMLResource doc " + SERVER_VERSION_XML + " failed:" + ex.toString(), log);
                throw new MRException(MRExceptionCodes.XML_NOT_VALID, MRXMLUtils.generateErrorXML("xsdFilePath", SERVER_VERSION_XML));
            }

            try {
                SERVER_VERSION = doc.getRootElement().attributeValue("Ver");
                System.out.println(" SERVER_VERSION " + SERVER_VERSION);
            } catch (Exception ex) {
                log(MRLog.INFO, MRLog.OK, "loadXMLResource doc " + SERVER_VERSION_XML + " failed:" + ex.toString(), log);
                throw new MRException(MRExceptionCodes.SERVICE_VERSION_XML_NOT_VALID, MRXMLUtils.generateErrorXML("error", "loadXMLResource url '" + SERVER_VERSION_XML + "' failed"));
            }

            if (SERVER_VERSION == null || SERVER_VERSION.trim().equals("")) {
                log(MRLog.INFO, MRLog.OK, "Failed to read service version", log);
                SERVER_VERSION = null;
                throw new MRException(MRExceptionCodes.SERVICE_VERSION_XML_NOT_VALID, MRXMLUtils.generateErrorXML("error", "loadXMLResource url '" + SERVER_VERSION_XML + "' failed"));
            }
        }
        return SERVER_VERSION;
    }

    ////////// uitlity funcations //////////////
    public static Element getDescendentWithoutNamespace(Element parent, String name) throws Exception {
        if (name.equals(parent.getName())) {
            return parent;
        }
        List<?> children = parent.elements();
        Iterator<?> i = children.iterator();
        Element element = null;
        while (i.hasNext()) {
            element = MRXMLUtils.getDescendentWithoutNamespace((Element) i.next(), name);
            if (element != null) {
                break;
            }
        }
        return element;
    }

    public static String getElementName(Element element, String defaultValue) {
        if (element != null) {
            return element.getName();
        }
        return defaultValue;
    }

    public static String getElementNameSpaceURI(Element element, String defaultValue) {
        if (element != null) {
            return element.getNamespaceURI();
        }
        return defaultValue;
    }

    public static final Element removeSystemAttributesFromXML(String inputXML) throws MRException, Exception {
        Document inputDoc = MRXMLUtils.stringToDocument(inputXML);
        String[] systemXpathList = { "//@NID", "//@Rev", "//@Ver", "//@parentNID", "//@serviceVer" };
        for (String attXpath : systemXpathList) {
            List<?> elementList = MRXMLUtils.getListXpath(inputDoc, attXpath);
            Iterator<?> elementListIterator = elementList.iterator();
            Attribute nidAttribute = null;
            while (elementListIterator.hasNext()) {
                nidAttribute = (Attribute) elementListIterator.next();
                nidAttribute.detach();
            }
        }
        return (Element) inputDoc.getRootElement().detach();
    }

     
 
    ////////////// error and log //////////

    public static String buildXPath(String tagName, String value) {
        return "(" + tagName + " = '" + value + "')";
    }

    public static String buildXPath(String tagName, String value, String operator) {
        return "(" + tagName + operator + "'" + value + "')";
    }

    public static String generateXML(String tagName, String value) {
        return "<" + tagName + ">" + value + "</" + tagName + ">";
    }

    public static String generateErrorXML(String tagName, String value) {
        return "<error> <" + tagName + "> " + value + "</" + tagName + "> </error>";
    }

    public static String generateErrorXMLTags(String tagName1, String value1, String tagName2, String value2) {
        return "<error> <" + tagName1 + "> " + value1 + "</" + tagName1 + "><" + tagName2 + ">" + value2 + "</" + tagName2 + "></error>";
    }

    private static void log(int level, int code, String data, MRLog log) {
        if (log != null) {
            log.addLog(level, code, data);
            System.out.println(data);
        } else {
            System.out.println(data);
        }

    }

    /////////////// similar old functions //////////
    public static Element addChildElement(Element parent, Element child) throws Exception {
        //parent.addContent(child.cloneContent());
        //return child;
        copyContent(parent, child);
        return child;        
    }

    public static Element addElementsFromDocument(Element parent, Document fromDoc, String xPath) throws Exception {
        List<?> elementList = getListXpath(fromDoc, xPath);
        Iterator<?> elementListIT = elementList.iterator();
        while (elementListIT.hasNext()) {
            MRXMLUtils.addElement(parent, (Element) ((Element) elementListIT.next()).clone());
        }
        return parent;
    }

    public static Element addElementFromOtherDoc(Element parent, Element newChild) throws Exception {
        //parent.addContent((Element) newChild.clone());
        //return newChild;
        copyElement(parent, newChild);
        return newChild;
    }

    public static String elementChildrenToString(Element parent) throws MRException, Exception {
        List<?> childerList = parent.elements();
        if (childerList.isEmpty()) {
            return MRXMLUtils.getValue(parent);
        }
        Iterator<?> childrenListIterator = childerList.iterator();
        StringBuffer buffer = new StringBuffer();
        Element childEL = null;
        while (childrenListIterator.hasNext()) {
            childEL = (Element) childrenListIterator.next();
            buffer.append(MRXMLUtils.elementToString(childEL));
        }
        return buffer.toString();
    }

    public static Element addChildrenToElement(Element parentEL, Element childELParent) throws MRException, Exception {
        List<?> childerList = childELParent.elements();
        return addChildrenToElement(parentEL, childerList);

    }

    public static Element addChildrenToElement(Element parentEL, List<?> childerList) throws MRException, Exception {
        if (childerList.isEmpty()) {
            return parentEL;
        }
        Iterator<?> childrenListIterator = childerList.iterator();
        Element childEL = null;
        while (childrenListIterator.hasNext()) {
            childEL = (Element) childrenListIterator.next();
            MRXMLUtils.addElement(parentEL, (Element) childEL.clone());
        }
        return parentEL;
    }

    ///////////////// new functions ///////////
    public static Element getParentElement(Element el) {
        return el.getParent();
    }

    public static Element getChild(Element el, String name) {
        return el.element(name);
    }

    public static void removeChild(Element el, String name) {
        Element child = el.element(name);
        if (child != null) {
            child.detach();
        }
    }
    @SuppressWarnings("unchecked")
    public static void removeChildren(Element el, String name) {
        List<Element> childlist = (List<Element>)el.elements(name);
        for (Element object : childlist) {
            object.detach();
        }
    }    
    public static void removeChild(Element el, String parentname, String childname) {
        Element child = el.element(parentname);
        if (child != null) {
            Element child2 = child.element(childname);
            if (child2 != null) {
                child2.detach();
            }
        }
    }
    
    public static void removeNodeXpath(Document doc, String xpath) throws Exception {
    	List<?> nodeList = getListXpath(doc, xpath);
    	for(Object object:nodeList) {
    		Node node = (Node)object;
    		node.detach();
    	}
    		
    }
    
    public static void nameSpaceRemoveNodeXpath(Node node, String name) throws Exception {
    	
    	if(node.getNodeType()== Node.ELEMENT_NODE){
    		
    		if(node.getName().equals(name)){
    			node.detach();
    			return;
    		} 
    		
    		List<?> nodeList = ((Element)node).content();
        	if(nodeList == null || nodeList.isEmpty()) {
        		return;
        	}
        	for(Object object:nodeList) {
        		Node node1 = (Node)object;
        		nameSpaceRemoveNodeXpath(node1, name);
        	}
    	} else if(node.getNodeType() == Node.ATTRIBUTE_NODE){
    		Attribute attribute = (Attribute)node;
    		if(attribute.getName().equals(name)){
    			attribute.detach();
    		}  
    	}
    	
    		
    }

    @SuppressWarnings("unchecked")
    public static List<Element> getChildren(Element parent){
        if (parent != null) {
            return (List<Element>)parent.elements();
        }
        return new ArrayList<Element>();
    }
    @SuppressWarnings("unchecked")
    public static List<Element> getChildren(Element parent, String name){
        if (parent != null) {
            return (List<Element>)parent.elements(name);
        }
        return new ArrayList<Element>();
    }
     

    ///////////// ad content/ element...these functions do not do explicit clone  //////////


    public static void addContent(Element to, Element content) throws Exception {
        to.add(content);
    }

    ////content related //////////
    public static List<?> cloneContent(Element to) throws Exception {
        return to.content();
    }

    public static Element removeContent(Element to) throws Exception {
        to.clearContent();
        return to;
    }

    //replace all children with other element's children
    public static Element replaceContent(Element to, Element from) throws Exception {
        to.clearContent();
        return copyContent(to, from); 
    }

    public static Element replaceContent(Element to, Element from, String from_child_xpath) throws Exception {
        to.clearContent();
        return copyContent(to, from, from_child_xpath);  
    }

    public static Element replaceContent(Element to, Document fromDoc, String from_child_xpath) throws Exception {
        to.clearContent();
        return copyContent(to, fromDoc, from_child_xpath);  
    }
    public static Element replaceContent(Element to, String to_child_xpath, Element from) throws Exception {
        Element toChild = getElementXpath(to, to_child_xpath);
        if (toChild != null) {
            to.clearContent();
            copyContent(toChild, from); 
        }
        return toChild;
    }

    public static Element replaceContent(Document toDoc, String to_child_xpath, Element from) throws Exception {
        Element toChild = getElementXpath(toDoc, to_child_xpath);
        if (toChild != null) {
            toChild.clearContent();
            copyContent(toChild, from); 
        }
        return toChild;
    }
    //replace all children with other element 
    public static Element replaceContentWithElement(Element to, Element from, boolean clone) throws Exception {
        to.clearContent();
        if (clone) {
            return copyElement(to, from);
        } else {
            addElement(to, from);
        }
        return to;
    }      
    public static Element replaceContentWithElement(Element to, Element from) throws Exception {
        to.clearContent();
        return copyElement(to, from);
    }

    public static Element replaceContentWithElement(Element to, Element from, String from_child_xpath) throws Exception {
        to.clearContent();
        return copyElement(to, from, from_child_xpath);
    }

    public static Element replaceContentWithElement(Element to, Document fromDoc, String from_child_xpath) throws Exception {
        to.clearContent();
        return copyElement(to, fromDoc, from_child_xpath);
    }
    
    //copy all children with other element's children
    @SuppressWarnings("unchecked")
    public static Element addContent(Element to, List<?> content) throws Exception {
        to.content().addAll(content);
        //for(int i=0; i<child.content().size(); i++){
        //    parent.add( (Node)((Node)child.content().get(i) ).clone() );
        //}
        return to;
    }
    public static Element copyContent(Element to, Element from) throws Exception {
    	if(from!=null)
            addContent(to, ((Element)from.clone()).content());
        return to;
    }

    public static Element copyContent(Element to, Element from, String from_child_xpath) throws Exception {
        Element fromChild = getElementXpath(from, from_child_xpath);
        if (fromChild != null) {
            copyContent(to, fromChild);
        }
        return to;
    }

    public static Element copyContent(Element to, Document fromDoc, String from_child_xpath) throws Exception {
        Element fromChild = getElementXpath(fromDoc, from_child_xpath);
        if (fromChild != null) {
            copyContent(to, fromChild);
        }
        return to;
    }
    
    public static Element copyContent(Element to, String to_child_xpath, Element from) throws Exception {
        Element toChild = getElementXpath(to, to_child_xpath);
        if (toChild != null) {
            copyContent(toChild, from);
        }
        return toChild;
    }

    public static Element copyContent(Document toDoc, String to_child_xpath, Element from) throws Exception {
        Element toChild = getElementXpath(toDoc, to_child_xpath);
        if (toChild != null) {
            copyContent(toChild, from);
        }
        return toChild;
    } 
    //copy element from other doc
    public static Element copyElement(Element to, Element from) throws Exception {
        to.add((Element) from.clone());
        return to;
    }

    public static Element copyElement(Element to, Element from, String from_child_xpath) throws Exception {
        Element fromChild = getElementXpath(from, from_child_xpath);
        if (fromChild != null) {
            copyElement(to, fromChild);
        }
        return to;
    }

    public static Element copyElement(Element to, Document fromDoc, String from_child_xpath) throws Exception {
        Element fromChild = getElementXpath(fromDoc, from_child_xpath);
        if (fromChild != null) {
            copyElement(to, fromChild);
        }
        return to;
    }
    public static Element copyElement(Document toDoc, String to_child_xpath, Element from) throws Exception {
        Element toChild = getElementXpath(toDoc, to_child_xpath);
        if (toChild != null) {
             copyElement(toChild, from);
        }
        return toChild;
    }   
    
    public static XPath  createXPath(String xpathExpression) {
        XPath xpathobj = DocumentHelper.createXPath(xpathExpression) ;
        return xpathobj;
    }
    
    /*
    public static XPath  createXPath(String xpathExpression, String namespacePrefix, String namesapceuri) {
        HashMap<String,String> map = new HashMap<String,String>();
        //> map.put("vra",   "http://dlib.york.ac.uk/vra4york");
        //> map.put(namespacePrefix, namesapceuri);  
        //map.put("vra",   "http://dlib.york.ac.uk/vra4york");
        map.put(namespacePrefix, namesapceuri);
        XPath xpathobj = DocumentHelper.createXPath(xpathExpression) ;
        xpathobj.setNamespaceContext( new SimpleNamespaceContext( map));
        return xpathobj;
    }
    */
    
    public static boolean compareElement(Element element1,Element element2) throws MRException,Exception {
    	if (element1 ==null || element2 ==null) {
    		return false;
    	}
    	NodeComparator nodeComparator = new NodeComparator();
    	int result = nodeComparator.compare(element1, element2);
    	System.out.println("result :="+result);
    	if(result == 0) {
    		return true;
    	}
    	return false;
    }
    
    public static Element getExceptionElement(int code, String description) throws Exception
    {
    	Element exceptionEl = MRXMLUtils.newElement(SystemServiceConst.EXCEPTION);
        Element codeEl = MRXMLUtils.newElement(SystemServiceConst.CODE);
        Element descriptionEl = MRXMLUtils.newElement (SystemServiceConst.DESCRIPTION);
        MRXMLUtils.addElement(exceptionEl, codeEl);
        MRXMLUtils.addElement(exceptionEl, descriptionEl);
        
        MRXMLUtils.setValueXpath(exceptionEl, SystemServiceConst.CODE, Integer.toString(code));
		MRXMLUtils.setValueXpath(exceptionEl, SystemServiceConst.DESCRIPTION, description);
		
		return exceptionEl;
    }
    
    public static void removeNameSpace(Element element) {
    	if(element ==null) {
    		return;
    	}
    	element.remove(element.getNamespace());
    	element.getNamespace().detach();
    	element.setQName(QName.get(element.getName(), Namespace.NO_NAMESPACE));
    	List<?> list = element.elements();
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element object = (Element) iterator.next();
            removeNameSpace(object);
        } 
    }
    
  
}
