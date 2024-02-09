/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.security.spec.AlgorithmParameterSpec;
import java.security.spec.KeySpec;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Locale;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.PBEParameterSpec;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.AttributesImpl;

import com.barco.nimbus.license.api.Feature;
import com.barco.nimbus.license.api.License;
import com.barco.nimbus.license.api.LicenseException;
import com.barco.nimbus.license.api.LicensingConstants;
import com.barco.nimbus.utils.common.XMLUtils;

/**
 * @author JMRA
 * 
 */
public class LicenseServiceUtils {

	private static final Logger mLogger = Logger
			.getLogger(LicenseServiceUtils.class);
	private static final String DEVICE_SERIAL_ID_GENERATOR_SCRIPT_PATH = "/etc/scripts/retrieve_serial_number.sh";
	private static final String LICENSE_TEMP_FILE_NAME = "templicensefile.xml";
	private static final String LICENSE_TEMP_FILE_PATH = (System
			.getenv("BLM_DYNAMIC_DIR") == null ? System
			.getProperty("java.io.tmpdir") : System.getenv("BLM_DYNAMIC_DIR"))
			+ "/" + LICENSE_TEMP_FILE_NAME;
	private static final String BARCO_SECRET_KEY = "b4rc0,BCD,0at3n";
	// 8-byte Salt
	private static final byte[] BARCO_SECRET_SALT = { (byte) 0xB1, (byte) 0x9A,
			(byte) 0x02, (byte) 0x8C, (byte) 0x03, (byte) 0x37, (byte) 0x14,
			(byte) 0x73 };

	private static final String BARCO_SELECTED_SECRET_KEY_ALGORITHM = "PBEWithMD5AndDES";
	public static final String CHAR_SET = "UTF-8";
	private static final int BARCO_PBEKEYSPEC_ITERATIONS_COUNT = 100;
	private static final int BARCO_PBEKEYSPEC_KEY_LENGTH = 128;

	public static License getLicense(Document xmlDocument) throws LicenseException, Exception{

		License license = new License();
		// Read Input XML and get data
		Element licenseDataEle = XMLUtils.getElementXpath(xmlDocument, "//"
				+ LicensingConstants.LICENSE_DATA);

		// Process license elements & its attributes
		Element licenseEle = XMLUtils.getElementXpathFromElement(
				licenseDataEle, licenseDataEle.getUniquePath() + "/"
						+ LicensingConstants.LICENSE);
		String licenseKey = XMLUtils.getAttributeValue(licenseEle,
				LicensingConstants.LICENSE_KEY);
		String deviceId = XMLUtils.getAttributeValue(licenseEle,
				LicensingConstants.ID);
		String licenseType = XMLUtils.getAttributeValue(licenseEle,
				LicensingConstants.LICENSE_TYPE);

		// Process product element & its attributes
		Element productEle = XMLUtils.getElementXpathFromElement(licenseEle,
				licenseEle.getUniquePath() + "/" + LicensingConstants.PRODUCT);
		String productId = XMLUtils.getAttributeValue(productEle,
				LicensingConstants.ID);
		String productName = XMLUtils.getValueXpath(productEle, "./"
				+ LicensingConstants.NAME, "");
		String revision = XMLUtils.getValueXpath(productEle, "./"
				+ LicensingConstants.REVISION, "");
		String division = XMLUtils.getValueXpath(productEle, "./"
				+ LicensingConstants.DIVISION, "");
		String version = XMLUtils.getValueXpath(productEle, "./"
				+ LicensingConstants.VERSION, "");

		// process customer element & its attributes
		Element customerEle = XMLUtils.getElementXpathFromElement(licenseEle,
				licenseEle.getUniquePath() + "/" + LicensingConstants.CUSTOMER);
		String customerId = XMLUtils.getAttributeValue(customerEle,
				LicensingConstants.ID);
		String customerName = XMLUtils.getValueXpath(customerEle, "./"
				+ LicensingConstants.NAME, "");

		// process project element & its attributes
		Element projectEle = XMLUtils.getElementXpathFromElement(licenseEle,
				licenseEle.getUniquePath() + "/" + LicensingConstants.PROJECT);
		String projectId = XMLUtils.getAttributeValue(projectEle,
				LicensingConstants.ID);
		String projectName = XMLUtils.getValueXpath(projectEle, "./"
				+ LicensingConstants.NAME, "");

		// process validity element & its attributes
		ValidityInfo vi = getValidityInfo(licenseEle);

		String licStartDate = vi.getStartDate();
		String licExpiryDate = vi.getExpiryDate();

		if (licExpiryDate == null || licExpiryDate.isEmpty()) {
			throw new LicenseException(
					"License expiration date can't be null or empty.");
		}

		// Throws exception if required field value are null
		// TODO: REMEMBER ME: Please all required values to this condition, to
		// make sure license generation will work without fail.
		if (licenseKey == null || deviceId == null || division == null
				|| revision == null || projectId == null || customerId == null) {
			throw new LicenseException(
					"One of required fields are missing, LicenseKey, DeviceId, Division, Revision, ProjectId and CustomerId");
		}

		license.setKey(licenseKey);
		license.setDeviceId(deviceId);
		license.setLicenseType(licenseType);
		license.setProductID(productId);
		license.setProductName(productName);
		license.setRevision(revision);
		license.setDivision(division);
		license.setProductVersion(version);
		license.setCustomerID(customerId);
		license.setCustomerName(customerName);
		license.setProjectID(productId);
		license.setProjectName(projectName);
		license.setStartDate(licStartDate);
		license.setExpiryDate(licExpiryDate);

		// Feature element
		Element featuresEle = XMLUtils.getElementXpathFromElement(licenseEle,
				licenseEle.getUniquePath() + "/" + LicensingConstants.FEATURES);
		List<?> featureElements = new ArrayList<Element>();
		if (featuresEle != null) {
			featureElements = XMLUtils.selectNodesXpath(featuresEle,
					featuresEle.getUniquePath() + "/"
							+ LicensingConstants.FEATURE);
			if (featureElements != null) {
				for (Object fe : featureElements) {

					Feature feature = new Feature();

					Element featureEle = (Element) fe;
					String featureId = XMLUtils.getAttributeValue(featureEle,
							LicensingConstants.ID);
					String featureName = XMLUtils.getAttributeValue(featureEle,
							LicensingConstants.NAME);

					// String qty = "0";
					// process validity element & its attributes
					vi = getValidityInfo(featureEle);
					String feaStartDate = vi.getStartDate();
					String feaExpiryDate = vi.getExpiryDate();

					// If feature doesn't specify any expiration date then it
					// will
					// be initialized with license expiration date.
					if (feaExpiryDate == null || feaExpiryDate.isEmpty()) {
						feaExpiryDate = licExpiryDate;
					}

					List<?> attributeElements = XMLUtils.selectNodesXpath(
							featureEle, featureEle.getUniquePath() + "/"
									+ LicensingConstants.ATTRIBUTE);

					if (attributeElements != null) {
						for (Object ae : attributeElements) {

							Element attributeEle = (Element) ae;

							if (attributeEle != null) {
								String propertyName = XMLUtils
										.getAttributeValue(attributeEle,
												LicensingConstants.NAME);
								String propertyValue = XMLUtils.getValueXpath(
										attributeEle, "//"
												+ LicensingConstants.ATTRIBUTE
												+ "[@name='" + propertyName
												+ "']", "");

								if (propertyName != null) {
									if (LicensingConstants.QUANTITY
											.equalsIgnoreCase(propertyName)
											&& propertyValue != null) {
										feature.setQTY(Integer
												.parseInt(propertyValue));
									} else {
										feature.getAttributesMap().put(
												propertyName, propertyValue);
									}
								}
							}
						}
					}

					feature.setID(featureId);
					feature.setName(featureName);
					feature.setStartDate(feaStartDate);
					feature.setExpiryDate(feaExpiryDate);

					license.getFeatures().add(feature);
				}
			}
		}
		return license;
	}

	public static Document getLicenseXMLDocument(License aLicense) {
		return getXMLDocument(aLicense, Boolean.TRUE);
	}

	public static Document getLicenseXMLDocumentForUser(License aLicense) {
		return getXMLDocument(aLicense, Boolean.FALSE);
	}
	
	private static Document getXMLDocument(License aLicense, Boolean encrypted)
	{
		Document xmlDoc = null;
		XMLWriter writer = new XMLWriter();
		try {
			writer.open(LICENSE_TEMP_FILE_PATH);
			writeLicenseToXML(aLicense, writer, encrypted);
			writer.close();
			xmlDoc = loadXMLResource(LICENSE_TEMP_FILE_PATH);
			// clean up
			new File(LICENSE_TEMP_FILE_PATH).delete();
		} catch (Exception e) {
			mLogger.error(
					"Exception occured while creating document from license: ",
					e);
		}
		return xmlDoc;
	}

	/**
	 * Read "Resources\TrialLicense.xml" and generate License out of it. TODO:
	 * remove after decision reading from .xml or .lic
	 * 
	 * @return License
	 */
	public static License getLicense() {
		License license = null;
		try {
			InputStream in = LicenseServiceUtils.class
					.getResourceAsStream("/resources/TrialLicense.xml");
			Document xmlDocument = loadXMLResourceFromStream(in);
			if (xmlDocument != null) {
				license = getLicense(xmlDocument);
			}
		} catch (Exception e) {
			mLogger.trace("Exception occured while loading trial license", e);
		}

		return license;
	}

	public static String getLicenseNID(Document xmlDocument) throws Exception {
		// Read Input XML and get data
		Element licenseDataEle = XMLUtils.getElementXpath(xmlDocument, "//"
				+ LicensingConstants.LICENSE_DATA);

		return XMLUtils.getAttributeValue(licenseDataEle,
				LicensingConstants.LICENSE_DATA_NID);
	}

	/**
	 * @param aTextToBeEncrypted
	 * 
	 * @return encrypted string for given input
	 */
	public static String getEncryptedString(String aTextToBeEncrypted) {
		StringBuilder sb = new StringBuilder();
		try {
			SecretKeyFactory factory = SecretKeyFactory
					.getInstance(BARCO_SELECTED_SECRET_KEY_ALGORITHM);
			KeySpec kSpec = new PBEKeySpec(BARCO_SECRET_KEY.toCharArray(),
					BARCO_SECRET_SALT, BARCO_PBEKEYSPEC_ITERATIONS_COUNT,
					BARCO_PBEKEYSPEC_KEY_LENGTH);
			SecretKey sKey = factory.generateSecret(kSpec);

			AlgorithmParameterSpec paramSpec = new PBEParameterSpec(
					BARCO_SECRET_SALT, BARCO_PBEKEYSPEC_ITERATIONS_COUNT);
			Cipher encrip = Cipher.getInstance(sKey.getAlgorithm());
			encrip.init(Cipher.ENCRYPT_MODE, sKey, paramSpec);
			byte[] encodedStrBytes = encrip.doFinal(aTextToBeEncrypted
					.getBytes());
			for (Byte b : encodedStrBytes) {
				sb.append(b).append(":");
			}
		} catch (Exception aneException) {
			mLogger.error(
					"Expiration date unable to encryped please try again",
					aneException);
		}

		return sb.toString();
	}

	/**
	 * @param anEncryptedText
	 * @return decrypted string for given encrypted string
	 */
	private static String getDecryptedString(String encryptedString) {
		String decryptedStr = null;
		try {
			byte[] bytes = new byte[encryptedString.split(":").length];
			int inx = 0;
			for (String s : encryptedString.split(":")) {
				bytes[inx] = Byte.parseByte(s);
				inx++;
			}
			KeySpec keySpec = new PBEKeySpec(BARCO_SECRET_KEY.toCharArray(),
					BARCO_SECRET_SALT, BARCO_PBEKEYSPEC_ITERATIONS_COUNT,
					BARCO_PBEKEYSPEC_KEY_LENGTH);
			SecretKey key = SecretKeyFactory.getInstance(
					BARCO_SELECTED_SECRET_KEY_ALGORITHM)
					.generateSecret(keySpec);
			AlgorithmParameterSpec paramSpec = new PBEParameterSpec(
					BARCO_SECRET_SALT, BARCO_PBEKEYSPEC_ITERATIONS_COUNT);
			Cipher decrip = Cipher.getInstance(key.getAlgorithm());
			decrip.init(Cipher.DECRYPT_MODE, key, paramSpec);
			decryptedStr = new String(decrip.doFinal(bytes), CHAR_SET);
		} catch (Exception aneException) {
			mLogger.error(
					"Expiration date format changed, Please upload license again",
					aneException);
			throw new LicenseException(
					"Expiration date format changed, Please upload license again",
					aneException);
		}
		return decryptedStr;
	}

	public static String getDeviceID() {
		// TODO: for the moment we are hard coding "1234567890" value for
		// windows, as there is no device id generation script available.
		String result = "1234567890";
		try {
			if ((System.getProperty("os.name")).equalsIgnoreCase("Linux")) {
				String temp = "";
				Process p = Runtime.getRuntime().exec(
						DEVICE_SERIAL_ID_GENERATOR_SCRIPT_PATH);
				BufferedReader input = new BufferedReader(
						new InputStreamReader(p.getInputStream()));
				String line;
				while ((line = input.readLine()) != null) {
					temp += line;
				}
				input.close();

				result = temp;
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return result.trim();
	}

	/**
	 * @param aDate
	 * @param aDateFormat
	 * 
	 * @return date for given date string and date format
	 */
	public static Date getDateFromString(String aDate, String aDateFormat) {

		// TODO: handle aDate string with never value.
		Date dateFromBLM = null;
		if (aDateFormat == null)
			aDateFormat = LicensingConstants.LICENSE_GENERAL_DATE_FORMAT;

		DateFormat formatter = new SimpleDateFormat(aDateFormat, Locale.US);
		try {
			dateFromBLM = formatter.parse(aDate);
		} catch (ParseException e) {
			mLogger.warn("Date format exception", e);
		}
		return dateFromBLM;
	}

	/**
	 * @param aDate
	 *            - date in string format
	 * @param aDateInputFormat
	 *            - aDate is in aDateInputFormat
	 * @param aDateOutputFormat
	 *            - return date sting will be in aDateOutputFormat
	 * 
	 * @return string format for given date string and output date format
	 */
	public static String getDateFromString(String aDate,
			String aDateInputFormat, String aDateOutputFormat) {
		Date dateFromBLM = null;
		String dateInString = null;

		if (aDateInputFormat == null)
			aDateInputFormat = LicensingConstants.LICENSE_GENERAL_DATE_FORMAT;

		if (aDateOutputFormat == null)
			aDateOutputFormat = LicensingConstants.LICENSE_GENERAL_DATE_FORMAT;

		DateFormat formatter = new SimpleDateFormat(aDateInputFormat, Locale.US);
		try {
			dateFromBLM = formatter.parse(aDate);
			formatter = new SimpleDateFormat(aDateOutputFormat, Locale.US);
			dateInString = formatter.format(dateFromBLM);
		} catch (ParseException e) {
			mLogger.warn("Date format exception", e);
		}

		return dateInString;
	}

	/**
	 * @param aDate
	 * @param aDateFormat
	 *            format for return string
	 * @return string format for given date and output date format
	 */
	public static String getDateFromString(Date aDate, String aDateFormat) {
		if (aDateFormat == null)
			aDateFormat = LicensingConstants.LICENSE_GENERAL_DATE_FORMAT;

		DateFormat formatter = new SimpleDateFormat(aDateFormat, Locale.US);
		return formatter.format(aDate);
	}

	private static ValidityInfo getValidityInfo(Element licOrFeatureEle)
			throws Exception {
		Element validityEle = XMLUtils.getElementXpathFromElement(
				licOrFeatureEle, licOrFeatureEle.getUniquePath() + "/"
						+ LicensingConstants.VALIDITY);
		String startDate = XMLUtils.getValueXpath(validityEle, "./"
				+ LicensingConstants.START_DATE, "");

		String expiryDate = XMLUtils.getValueXpath(validityEle, "./"
				+ LicensingConstants.EXPIRY_DATE, "");
		if (expiryDate == null || expiryDate.isEmpty()) {
			Element durationEle = XMLUtils.getElementXpathFromElement(
					validityEle, validityEle.getUniquePath() + "/"
							+ LicensingConstants.DURATION);

			if (durationEle != null) {
				String years = XMLUtils.getValueXpath(durationEle, "./"
						+ LicensingConstants.YEARS, "");
				String months = XMLUtils.getValueXpath(durationEle, "./"
						+ LicensingConstants.MONTHS, "");
				String days = XMLUtils.getValueXpath(durationEle, "./"
						+ LicensingConstants.DAYS, "");

				Calendar cal = Calendar.getInstance();
				cal.setTime(new Date());
				cal.add(Calendar.YEAR, Integer.parseInt(years));
				cal.add(Calendar.MONTH, Integer.parseInt(months));
				cal.add(Calendar.DATE, Integer.parseInt(days));
				expiryDate = getDateFromString(cal.getTime(), null);
			}
		} else if (expiryDate
				.equalsIgnoreCase(LicensingConstants.EXPIRES_NEVER)) {
			expiryDate = LicensingConstants.EXPIRES_NEVER;
		}

		if (startDate == null || startDate.isEmpty()) {
			startDate = getDateFromString(new Date(), null);
		}

		return new LicenseServiceUtils.ValidityInfo(startDate, expiryDate);
	}

	/**
	 * @author JMRA
	 * 
	 */
	private static class ValidityInfo {
		private String startDate;
		private String expiryDate;

		public ValidityInfo(String startDate, String expiryDate) {
			this.startDate = startDate;
			this.expiryDate = expiryDate;
		}

		/**
		 * @return start date
		 */
		public String getStartDate() {
			return getDecryptedString(startDate);
		}

		/**
		 * @return expiration date
		 */
		public String getExpiryDate() {
			return getDecryptedString(expiryDate);
		}
	}

	/**
	 * Writes license in XML
	 * 
	 * @param aStream
	 * @throws SAXException
	 *             occurs while calling beginElement or writeProperty on
	 *             XMLPrinter object.
	 */
	private static void writeLicenseToXML(License aLicense, XMLWriter aStream,
			Boolean encrypted) throws SAXException {
		assert (aLicense != null) : "License is null while writing to XML";
		assert (aStream != null) : "XMLPrinter  is null while writing to XML";
		if (aLicense == null || aStream == null) {
			return;
		}
		mLogger.debug("Write License " + aLicense);
		AttributesImpl attribs = new AttributesImpl();
		attribs.addAttribute("", "", LicensingConstants.LICENSE_KEY,
				String.class.getName(), aLicense.getKey());
		attribs.addAttribute("", "", LicensingConstants.ID,
				String.class.getName(), aLicense.getDeviceId());
		attribs.addAttribute("", "", LicensingConstants.LICENSE_TYPE,
				String.class.getName(), aLicense.getLicenseType());
		aStream.beginElement(LicensingConstants.LICENSE, attribs);

		if (aLicense.getProductID() != null
				&& !aLicense.getProductID().trim().equals("")) {
			attribs = new AttributesImpl();
			attribs.addAttribute("", "", LicensingConstants.ID,
					String.class.getName(), aLicense.getProductID());
			aStream.beginElement(LicensingConstants.PRODUCT, attribs);
			aStream.writeProperty(LicensingConstants.NAME,
					aLicense.getProductName());
			aStream.writeProperty(LicensingConstants.DIVISION,
					aLicense.getDivision());
			aStream.writeProperty(LicensingConstants.VERSION,
					aLicense.getProductVersion());
			aStream.writeProperty(LicensingConstants.REVISION,
					aLicense.getRevision());
			aStream.endElement();
		}
		if (aLicense.getCustomerID() != null
				&& !aLicense.getCustomerID().trim().equals("")) {
			attribs = new AttributesImpl();
			attribs.addAttribute("", "", LicensingConstants.ID,
					String.class.getName(), aLicense.getCustomerID());
			aStream.beginElement(LicensingConstants.CUSTOMER, attribs);
			aStream.writeProperty(LicensingConstants.NAME,
					aLicense.getProductName());
			aStream.endElement();
		}
		if (aLicense.getProjectID() != null
				&& !aLicense.getProjectID().trim().equals("")) {
			attribs = new AttributesImpl();
			attribs.addAttribute("", "", LicensingConstants.ID,
					String.class.getName(), aLicense.getProjectID());
			aStream.beginElement(LicensingConstants.PROJECT, attribs);
			aStream.writeProperty(LicensingConstants.NAME,
					aLicense.getProjectName());
			aStream.endElement();
		}
		if (aLicense.getExpiryDate() != null
				&& !aLicense.getExpiryDate().trim().equals("")) {
			attribs = new AttributesImpl();
			aStream.beginElement(LicensingConstants.VALIDITY, attribs);
			if (!encrypted) {
				aStream.writeProperty(LicensingConstants.START_DATE,
						aLicense.getStartDate());
				aStream.writeProperty(LicensingConstants.EXPIRY_DATE,
						aLicense.getExpiryDate());
			} else {
				aStream.writeProperty(LicensingConstants.START_DATE,
						getEncryptedString(aLicense.getStartDate()));
				aStream.writeProperty(LicensingConstants.EXPIRY_DATE,
						getEncryptedString(aLicense.getExpiryDate()));
			}
			aStream.endElement();
		}

		if (!aLicense.getFeatures().isEmpty()) {
			attribs = new AttributesImpl();
			aStream.beginElement(LicensingConstants.FEATURES, attribs);
			for (Feature f : aLicense.getFeatures()) {
				attribs = new AttributesImpl();
				attribs.addAttribute("", "", LicensingConstants.ID,
						String.class.getName(), f.getID());
				attribs.addAttribute("", "", LicensingConstants.NAME,
						String.class.getName(), f.getName());
				aStream.beginElement(LicensingConstants.FEATURE, attribs);

				if (f.getExpiryDate() != null
						&& !f.getExpiryDate().trim().equals("")) {
					attribs = new AttributesImpl();
					aStream.beginElement(LicensingConstants.VALIDITY, attribs);
					if (!encrypted) {
						aStream.writeProperty(LicensingConstants.START_DATE,
								f.getStartDate());
						aStream.writeProperty(LicensingConstants.EXPIRY_DATE,
								f.getExpiryDate());
					} else {
						aStream.writeProperty(LicensingConstants.START_DATE,
								getEncryptedString(f.getStartDate()));
						aStream.writeProperty(LicensingConstants.EXPIRY_DATE,
								getEncryptedString(f.getExpiryDate()));
					}
					aStream.endElement();
				}

				if (!f.getAttributesMap().isEmpty()) {
					for (String key : f.getAttributesMap().keySet()) {
						attribs = new AttributesImpl();
						attribs.addAttribute("", "", LicensingConstants.NAME,
								String.class.getName(), key);
						aStream.writeProperty(LicensingConstants.ATTRIBUTE, f
								.getAttributesMap().get(key), attribs);
					}

				}
				aStream.endElement();
			}
			aStream.endElement();
		}
		aStream.endElement();
	}

	private static Document loadXMLResource(String xmlFilePath)
			throws Exception {
		InputStream source = new FileInputStream(new File(xmlFilePath));
		return loadXMLResourceFromStream(source);
	}

	private static Document loadXMLResourceFromStream(InputStream anInputStream) {
		Document licXMLDoc = null;
		try {
			BufferedReader in = new BufferedReader(new InputStreamReader(
					anInputStream, CHAR_SET));
			StringBuilder xmlAsStr = new StringBuilder();
			String line = null;
			while ((line = in.readLine()) != null) {
				xmlAsStr.append(line);
				xmlAsStr.append("\n");
			}
			in.close();
			licXMLDoc = DocumentHelper.parseText(xmlAsStr.toString());
		} catch (IOException e) {
			mLogger.warn("IOException in converting xml file to xml document ",
					e);
		} catch (DocumentException e) {
			throw new LicenseException(
					"Exception in converting xml file to xml document ", e);
		}

		return licXMLDoc;
	}
}
