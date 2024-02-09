package com.barco.nimbus.license.util;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.UnsupportedEncodingException;
import java.util.Stack;

import javax.xml.transform.OutputKeys;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.sax.SAXTransformerFactory;
import javax.xml.transform.sax.TransformerHandler;
import javax.xml.transform.stream.StreamResult;

import org.apache.log4j.Logger;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import com.barco.nimbus.license.api.LicensingConstants;

/**
 * To write into XML file, during export operation
 */
public class XMLWriter {
	private Logger mLogger = Logger.getLogger(XMLWriter.class);
	private Stack<String> mElements = new Stack<String>();

	private TransformerHandler mStream;
	private BufferedWriter mOut;

	/**
	 * This method will start writing Data during export operation to specified
	 * file
	 * 
	 * @param aFileName
	 * @throws FileNotFoundException
	 * @throws TransformerConfigurationException
	 * @throws SAXException
	 */
	public void open(String aFileName) throws FileNotFoundException,
			TransformerConfigurationException, SAXException,
			UnsupportedEncodingException {
		try {
			mOut = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(new File(aFileName)), LicenseServiceUtils.CHAR_SET));
			StreamResult result = new StreamResult(mOut);
			SAXTransformerFactory tf = (SAXTransformerFactory) TransformerFactory
					.newInstance();

			mStream = tf.newTransformerHandler();
			mStream.getTransformer().setOutputProperty(OutputKeys.ENCODING,
					LicenseServiceUtils.CHAR_SET);
			mStream.getTransformer()
					.setOutputProperty(OutputKeys.METHOD, "xml");
			mStream.getTransformer()
					.setOutputProperty(OutputKeys.INDENT, "yes");
			mStream.setResult(result);
			mStream.startDocument();

			beginElement(LicensingConstants.LICENSE_DATA, new AttributesImpl());

		} catch (IOException anException) {
			mLogger.warn("IOException While writing to stream:", anException);
		} catch (Exception anException) {
			mLogger.warn("Exception While writing to stream:", anException);
		}
	}

	/**
	 * It is called at the end of all file
	 * 
	 * @throws SAXException
	 */
	public void close() throws SAXException {
		endElement();
		mStream.endDocument();
		try {
			mOut.flush();
			mOut.close();
		} catch (IOException anException) {
			mLogger.warn("Exception While closing Connection:", anException);
		}
	}

	/**
	 * This method is called on every begin element
	 * 
	 * @param aName
	 * @throws SAXException
	 */
	public void beginElement(String aName, AttributesImpl anElementAttributes)
			throws SAXException {
		newLine();
		indent(mElements.size());

		mStream.startElement("", "", aName, anElementAttributes);
		mElements.push(aName);
	}

	/**
	 * This method is called on every end element.
	 * 
	 * @throws SAXException
	 */
	public void endElement() throws SAXException {
		newLine();
		indent(mElements.size() - 1);
		mStream.endElement("", "", mElements.pop());
	}

	/**
	 * To write the values of the each element
	 * 
	 * @param aName
	 * @param aValue
	 * @throws SAXException
	 */
	public void writeProperty(String aName, String aValue) throws SAXException {
		writeProperty(aName, aValue, new AttributesImpl());
	}

	/**
	 * To write the values of the each element
	 * 
	 * @param aName
	 * @param aValue
	 * @throws SAXException
	 */
	public void writeProperty(String aName, String aValue,
			AttributesImpl anAttributes) throws SAXException {
		beginElement(aName, anAttributes);
		if (aValue != null)
			mStream.characters(aValue.toCharArray(), 0, aValue.length());
		endElementNoIndent();
	}

	/**
	 * To write the values of the integer element
	 * 
	 * @param aName
	 * @param aValue
	 * @throws SAXException
	 */
	public void writeProperty(String aName, int aValue) throws SAXException {
		writeProperty(aName, Integer.toString(aValue));
	}

	private void newLine() throws SAXException {
		mStream.ignorableWhitespace(new char[] { '\n' }, 0, 1);
	}

	private void indent(int aCount) throws SAXException {
		char[] c = new char[aCount];
		for (int i = 0; i < c.length; i++)
			c[i] = '\t';
		mStream.ignorableWhitespace(c, 0, c.length);
	}

	private void endElementNoIndent() throws SAXException {
		mStream.endElement("", "", mElements.pop());
	}
}
