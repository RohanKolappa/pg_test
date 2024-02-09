package com.ipvsserv.nxd.xslt.extn;

import java.io.StringReader;
import java.io.StringWriter;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.stream.StreamResult;

import org.w3c.dom.Document;
import org.xml.sax.InputSource;

import com.ipvsserv.common.util.CompUtils;

public class DOMXmlUtils {
    
    public static String elementToString(org.w3c.dom.Node node) throws RuntimeException {
        String stringResult = null;
        try {
            Source source = new DOMSource(node);
            StringWriter stringWriter = new StringWriter();
            Result result = new StreamResult(stringWriter);
            TransformerFactory factory = TransformerFactory.newInstance();
            Transformer transformer = factory.newTransformer();
            transformer.transform(source, result);
            stringResult = stringWriter.getBuffer().toString();
            stringResult = CompUtils.removeXmlProlog(stringResult);
        } catch (TransformerConfigurationException e) {
            e.printStackTrace();
            throw new RuntimeException("Unable to instantiate XML Transformer");
        } catch (TransformerException e) {
            e.printStackTrace();
            throw new RuntimeException("Error in XML Transformer");
        }
        return stringResult;
    }

    public static Document stringToDocument(String xml) throws org.xml.sax.SAXException, java.io.IOException {
        InputSource is = new InputSource(new StringReader(xml));
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setNamespaceAware(true);
        DocumentBuilder builder = null;
        try {
            builder = factory.newDocumentBuilder();
            Document doc = builder.parse(is);
            return doc;
        } catch (ParserConfigurationException e) {
            e.printStackTrace();
            return null;
        }
    }

    public static Document inputStreamToDocument(java.io.InputStream is) throws org.xml.sax.SAXException, java.io.IOException {
        DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
        factory.setNamespaceAware(true);
        javax.xml.parsers.DocumentBuilder builder = null;
        try {
            builder = factory.newDocumentBuilder();
            Document doc = builder.parse(is);
            is.close();
            return doc;
        } catch (ParserConfigurationException ex) {
            ex.printStackTrace();
            return null;
        }
    }

}
