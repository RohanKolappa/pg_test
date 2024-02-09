package org.jivesoftware.openfire.user;

import java.io.File;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.Node;
import org.dom4j.QName;
import org.dom4j.io.SAXReader;


public class XMLUtils {
	
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
    
    public static Document getDocument(File file) {
        try {
            SAXReader xmlReader = new SAXReader();
            return xmlReader.read(file);
        } catch (Exception ex) {
           ex.printStackTrace();
        }
        return null;
    }

    public static Document loadXMLFile(String xmlFilePath) throws Exception {
        File file = null;
        Document doc = null;
        file = new File(xmlFilePath);
        if (!file.exists()) {
            throw new Exception("XML File not found " + xmlFilePath);
        }
        try {
            doc = XMLUtils.getDocument(file);
        } catch (Exception ex) {
            throw new Exception("XML File not found  or invalid." + xmlFilePath);
        }
        return doc;
    }
}
