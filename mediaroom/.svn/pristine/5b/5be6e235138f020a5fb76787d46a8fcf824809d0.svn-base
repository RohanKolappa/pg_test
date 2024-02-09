package com.ipvs.http.common;

import java.io.StringWriter;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerFactoryConfigurationError;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.dom4j.io.DOMReader;
import org.w3c.dom.Document;

public class HttpUtils {
	
	public static String documentToString(Document document) {
		
		try {
			TransformerFactory transfac = TransformerFactory.newInstance();
			Transformer trans = transfac.newTransformer();

			trans.setOutputProperty(OutputKeys.OMIT_XML_DECLARATION, "yes");
			trans.setOutputProperty(OutputKeys.INDENT, "yes");

			//create string from xml tree
			StringWriter sw = new StringWriter();
			StreamResult result = new StreamResult(sw);
			DOMSource source = new DOMSource(document);			
			trans.transform(source, result);
			return sw.toString();
		} catch (TransformerConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (TransformerFactoryConfigurationError e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (TransformerException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
		
	}
	
	public static org.dom4j.Document covertW3CDocumentToDom4JDocument(Document document) {
		DOMReader domReader  = new DOMReader();
		org.dom4j.Document dom4jDocument =  domReader.read(document);
		return dom4jDocument;
		

	}

}
