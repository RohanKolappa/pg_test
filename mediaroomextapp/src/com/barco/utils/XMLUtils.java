package com.barco.utils;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Node;

import com.barco.utils.mr.MRXMLUtils;

public class XMLUtils {

	public static Element getElementXpath(Document doc, String xpath) throws Exception {
		return (Element)doc.selectSingleNode(xpath);
	}

	public static String getValueXpath(Document doc, String xpath, String defaultValue) throws Exception {
		Node node = doc.selectSingleNode(xpath);
		if(node == null)
			return defaultValue;
		return node.getStringValue();
	}
	
	public static Document elementToDocument(Element element) throws Exception {
		Document doc = DocumentHelper.createDocument();
		doc.add(element);
		return doc;
	}
	
	public static Document stringToDocument(String s) throws Exception {
		return DocumentHelper.parseText(s);
	}
	
	public static Document fileToDocument(String filename) throws Exception {
		String xml = FileUtils.readFile(filename);
		return XMLUtils.stringToDocument(xml);
	}
}
