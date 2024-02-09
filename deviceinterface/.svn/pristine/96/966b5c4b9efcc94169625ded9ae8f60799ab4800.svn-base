package com.barco.utils;

import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Node;
import org.dom4j.XPath;

public class XMLUtils {
	
	public static Element getElementXpath(Document doc, String xpath) throws Exception {
		return (Element)doc.selectSingleNode(xpath);
	}

	public static Element getElementXpathFromElement(Element element, String xpath) throws Exception 
	{
		return (Element)element.selectSingleNode(xpath);
	}

	public static String getValueXpath(Document doc, String xpath, String defaultValue) throws Exception {
		Node node = doc.selectSingleNode(xpath);
		if(node == null)
			return defaultValue;
		return node.getStringValue();
	}
	
	public static String getValueXpath(Element element, String xpath, String defaultValue) throws Exception {
		Node node = element.selectSingleNode(xpath);
		if(node == null)
			return defaultValue;
		return node.getStringValue();
	}
	
	public static void setValueXpath(Document doc, String xpath, String value) throws Exception {
		Node node = doc.selectSingleNode(xpath);
		// TBD handle the null case
		if(node == null) {
			System.out.println("XMLUtils:setValueXpath node not found for xpath=" + xpath);
			(new Exception()).printStackTrace();
			return;
			
		}
		node.setText(value);
	}
	
	
	public static void setValueXpathElement(Element element, String xpath, String value) throws Exception {
		Node node = element.selectSingleNode(xpath);
		// TBD handle the null case
		if(node == null) {
			System.out.println("XMLUtils:setValueXpath node not found for xpath=" + xpath);
			(new Exception()).printStackTrace();
			return;
			
		}
		node.setText(value);
	}

    public static Document elementToDocument(Element parent) throws Exception {
        Element newRoot = parent.createCopy();
        Document doc = DocumentHelper.createDocument();
        doc.add(newRoot);
        return doc;        
    }
    
    public static String elementToString(Element parent) throws Exception {
    	if(parent != null) {
    		return parent.asXML();
    	}
    	return null;
    }
	
	public static Document stringToDocument(String s) throws Exception {
		return DocumentHelper.parseText(s);
	}
	
	public static Document fileToDocument(String filename) throws Exception {
		String xml = FileUtils.readFile(filename);
		return XMLUtils.stringToDocument(xml);
	}
	
	// TBD This should check that the attribute Exists, kind of a schema check
    public static Element addAttribute(Element parent, String name, String value)  {
        parent.addAttribute(name, value);
        return parent;
    }
    
    public static String getAttributeValue(Element parent, String attibutename) {
    	return parent.attributeValue(attibutename);
    }

    public static String getAttributeValueWithDefault(Element parent, String attibutename, String defaultValue) {
    	if(parent == null || parent.attributeValue(attibutename) == null) {
    		return defaultValue;
    	}
    	return parent.attributeValue(attibutename);
    }


    public static Element stringToElement(String xml) throws Exception {
        Document doc = stringToDocument(xml);

        if (doc != null) {
            return (Element) doc.getRootElement().clone();
        }

        return null;
    }

    public static String documentToString(Document doc) throws Exception {
    	return doc.asXML();
    }
        
    public static String generateErrorXML(String tagName, String value) {
        return "<error> <" + tagName + "> " + value + "</" + tagName + "> </error>";
    }

    public static List<?> selectNodesXpath(Element parent, String xpath) throws Exception {
        XPath xpathObj = DocumentHelper.createXPath(xpath);
        return xpathObj.selectNodes(parent);
    }
    
    public static List<?> getListOfNodesXpath(Document document, String xpath) throws Exception {
        XPath xpathObj = DocumentHelper.createXPath(xpath);
        return xpathObj.selectNodes(document);
    }
    
    public static String getValue(Element parent, String name, String defaultValue)  {
    	Element element = parent.element(name);
    	String value = element.getText();
    	if(value == null)
    		value = defaultValue;
    	return value;
    }

    public static Document loadXMLResource(String xmlFilePath) throws Exception {
    	URL url = XMLUtils.class.getResource(xmlFilePath);
    	if(url == null) {
    		//TODO: throw exception    		
    	}
    	// String xml = new Scanner(url.openStream(), "UTF-8").useDelimiter("\\A").next();
		InputStream source = url.openStream();
		byte [] buf = new byte[ source.available() ];
	    new DataInputStream( source ).readFully( buf );
	    String xml = new String( buf );	   
    	return DocumentHelper.parseText(xml);
    }
    
    @SuppressWarnings("unchecked")
	public static void replaceContent(Element to,Element from) throws Exception {
    	if(to ==null || from==null || !from.hasContent()){
    		return;
    	}
    	to.clearContent();
    	to.content().addAll(((Element)from.clone()).content());
    }


    public static Document createRequestDocument(String servicename, String requestname, String userJID, 
    		String requestNID, String clientdata,String clientcallback, String data) throws Exception {
        String xml = "<Request><Header serviceName='" + servicename + "' requestName='" + requestname + "'  userJID='" + userJID +
                "' requestNID='" + requestNID + "'><ClientData>" + clientdata + "</ClientData>" + "<ClientCallback>" + clientcallback + "</ClientCallback>"  + "</Header>" +
            "<Data>" + data  + "</Data>" + "</Request>";
        return DocumentHelper.parseText(xml);
    }
    
	public static long getLongValue(Document doc, String xpath, long defaultValue) {
		try {
			return Long.parseLong(XMLUtils.getValueXpath(doc, xpath, String.valueOf(defaultValue)));
		}
		catch(Exception e) {
			return defaultValue;
		}
	}

	public static int getIntValue(Document doc, String xpath, int defaultValue) {
		try {
			return Integer.parseInt(XMLUtils.getValueXpath(doc, xpath, String.valueOf(defaultValue)));
		}
		catch(Exception e) {
			return defaultValue;
		}
	}
	
	public static Element addElementXpath(Document parent, String to_child_xpath, String name, String value) throws Exception {
        Element e = getElementXpath(parent, to_child_xpath);
        if (e != null) {
            return addElement(e, name, value) ;
        }
        return null;
         
    }
    
    public static Element addElement(Element parent, String name, String value) {
        Element e = newElement(name);
        if (value != null) {
            e.setText(value);
        }
        parent.add(e);
        return e;
    }
    
    private static Element newElement(String name) {
        return DocumentHelper.createElement(name);
    }
}
