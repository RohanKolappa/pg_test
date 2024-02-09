/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license;

import java.io.FileNotFoundException;
import java.lang.reflect.Method;
import java.util.Date;

import javablmlib.BLM;
import javablmlib.BarcoLicense;

import org.apache.log4j.Logger;

import com.barco.nimbus.license.LicenseService.LicenseUtilityNames;
import com.barco.nimbus.license.api.Feature;
import com.barco.nimbus.license.api.License;
import com.barco.nimbus.license.api.LicenseException;
import com.barco.nimbus.license.api.LicenseStatus;
import com.barco.nimbus.license.api.LicensingConstants;
import com.barco.nimbus.license.util.FeatureSet;
import com.barco.nimbus.license.util.LicenseResultCodes;
import com.barco.nimbus.license.util.LicenseServiceUtils;

/**
 * This class acts like an adapter which 1. Unload current license and 2. Read
 * content from *.lic file using BLM module and returns license object.
 * 
 * @author JMRA
 */
public class LicenseAdaptor {

	private static final Logger mLogger = Logger
			.getLogger(LicenseAdaptor.class);
	private static final String HEART = "0";
	private static final String UNIFY = "";

	private License license = new License();

	/**
	 * Creates a new BLMWrapper. This will read in information from the license
	 * file (if available) and set the license status accordingly.
	 * 
	 * @throws IllegalStateException
	 *             in case loading of native libraries required by BLM or
	 *             gathering information to uniquely identify at BLM failed
	 */
	public LicenseAdaptor() throws IllegalStateException {
		// make sure that "JavaBLMLib" is loaded, as creating new instance of
		// BLM will unload library.
	}

	/**
	 * Release used BLM resources.
	 */
	public void terminate(License aLicense) {
		mLogger.trace("START: terminating License - " + aLicense);

		BLM blm = BLMSingleton.blmInstance.getBLMSingleton();

		if (BLMSingleton.blmInstance.getBLMLoadError() == -1) {
			mLogger.error("Unable to load BLM Library, can't terminate license - "
					+ aLicense);
			throw new IllegalStateException(
					"Unable to load BLM Library, can't terminate license - "
							+ aLicense);
		}

		// terminate license
		int returnCode = blm.LicenseEnd(aLicense.getLicenseType());

		if (returnCode == LicenseResultCodes.SUCCESS.getErrorCode())
			mLogger.trace("Licensed product terminated successfully.");
		else
			mLogger.trace("Failed to terminate license "
					+ aLicense.getLicenseType()
					+ ". "
					+ LicenseResultCodes.translate(returnCode)
							.getFriendlyName());

		// terminate all features of license
		for (Feature feature : aLicense.getFeatures()) {
			returnCode = blm.LicenseEnd(feature.getName());

			if (returnCode == LicenseResultCodes.SUCCESS.getErrorCode())
				mLogger.trace("License feature " + feature.getName()
						+ ", terminated successfully.");
			else
				mLogger.trace("Failed to terminate license feature "
						+ feature.getName()
						+ ". "
						+ LicenseResultCodes.translate(returnCode)
								.getFriendlyName());
		}

		mLogger.trace("END: terminating License - " + aLicense);
	}

	/**
	 * load the license file and update license information.
	 */
	public License readLicense(License anOldLicense) {
		mLogger.trace(LicenseAdaptor.class.getName() + ":readLicense - START");
		// Terminates old license before loading new license.
		if (anOldLicense.getExpiryDate() != null) {
			terminate(anOldLicense);
			mLogger.trace(LicenseAdaptor.class.getName()
					+ ":readLicense - terminated old license.");
		}
		license = anOldLicense;
		initialize();
		mLogger.trace(LicenseAdaptor.class.getName()
				+ ":readLicense - END: License Details: " + license);
		return license;
	}

	/*
	 * License will be initialized here
	 * 
	 * @param aLicense
	 */
	private void initialize() {
		LicenseStatus status = LicenseStatus.TrialLicense;

		try {
			// initialize products and set the granted number of licenses per
			// product where applicable
			initProduct();
			initProductFeatures();

			if (license.getStatus() == LicenseStatus.LicenseExpired) {
				status = LicenseStatus.LicenseExpired;
			} else {
				status = LicenseStatus.LicenseAccepted;
			}

			// Validate Device ID
			String deviceId = LicenseServiceUtils.getDeviceID();
			// Initialize to trial license if device id is "XXXXXX" or
			// "XXXXXX,XXXXXX"
			if ((license.getDeviceId().equals("XXXXXX") || license
					.getDeviceId().equals("XXXXXX,XXXXXX"))) {
				status = LicenseStatus.TrialLicense;
				if (deviceId != null && !deviceId.trim().equals(""))
					license.setDeviceId(deviceId);
			} else if (!deviceId.equals(license.getDeviceId())) {
				throw new LicenseException(
						"Invalid device found with"
								+ " DeviceID="
								+ license.getDeviceId()
								+ " while loading license. Please load license with correct deviceID");
			}
		} catch (FileNotFoundException e) {
			// set license status trial
			status = LicenseStatus.TrialLicense;
			mLogger.error(
					"Failed to initialize license, specified file not found. Initialized to TrialMode. ",
					e);
		} catch (LicenseException e) {
			// set license status to invalid
			status = LicenseStatus.LicenseInvalid;
			mLogger.error(
					"Exception occured failed to initialize license, set to InvalidLicense. ",
					e);
			throw e;
		}

		license.setStatus(status);
	}

	private void initProduct() throws FileNotFoundException, LicenseException {
		LicenseStatus status = null;
		BarcoLicense lic = new BarcoLicense();

		LicenseResultCodes code = initBLMLicense(license.getLicenseType(), lic);

		if (code != LicenseResultCodes.SUCCESS
				&& code != LicenseResultCodes.ERR_QTY) // quantity
		// <
		// 1
		// is
		// specified
		// for
		// the
		// feature
		{
			license.setStatus(LicenseStatus.LicenseInvalid);
			throw new LicenseException(
					"Error occurred while initializing license, invalid license file, Error: "
							+ code);
		}

		if (code == LicenseResultCodes.ERR_QTY) // quantity < 1 is specified for
												// the
		// feature
		{
			status = LicenseStatus.LicenseAccepted;
		}

		// BLM is not initializing BarcoLicense values internally, so we need to
		// do it manually.
		initializeBarcoLicenseManuallyFromBLM(license.getLicenseType(), lic);
		// provide an extra validation
		// status =
		// checkInitResult(licenseCheckCustom(license.getProductName()));
		license.setStatus(status);

		// extract meta information (project name, customer name, etc), which is
		// stored in CUSTOM String
		processCustomKeyString(lic.getCustom());

		updateLicense(lic);
	}

	private void initProductFeatures() throws FileNotFoundException,
			LicenseException {
		for (FeatureSet fSet : FeatureSet.values()) {
			initFeature(fSet.name());
		}
	}

	private void initFeature(String aProductName) throws FileNotFoundException,
			LicenseException {
		LicenseStatus status = null;
		Feature f = new Feature();
		BarcoLicense lic = new BarcoLicense();

		LicenseResultCodes resCode = initBLMLicense(aProductName, lic);
		if (resCode == LicenseResultCodes.LICENSE_STRING_MISSING) {
			// log for skpping feature, as loading license doesn't have this
			// feature.
			return;
		}

		if (resCode != LicenseResultCodes.SUCCESS
				&& resCode != LicenseResultCodes.ERR_QTY) // quantity < 1 is
															// specified for the
															// feature
		{
			status = LicenseStatus.LicenseInvalid;
			mLogger.info("Error occurred while initializing license Feature, invalid license file, Error: "
					+ resCode);
		}

		if (resCode == LicenseResultCodes.ERR_QTY) // quantity < 1 is specified
													// for
													// the feature
		{
			status = LicenseStatus.LicenseAccepted;
		}

		// BLM is not initializing BarcoLicense values internall, so we need to
		// do it manually.
		initializeBarcoLicenseManuallyFromBLM(aProductName, lic);
		f.setName(aProductName);
		// provide an extra validation here, like check deviceid, license key,
		// etc...
		// status = checkInitResult(licenseCheckExpiry(aProductName));
		f.setStatus(status);

		// extract meta information (featureId, StartDate, attributes, etc),
		// stored in feature CUSTOM String
		processFeatureCustomKeyString(f, lic.getCustom());

		updateFeature(f, lic);
		license.getFeatures().add(f);
	}

	private void updateLicense(BarcoLicense barcoLic) {
		license.setProductVersion(barcoLic.getVersion());
		license.setQTY(Integer.parseInt(((barcoLic.getQty() == null || barcoLic
				.getQty().isEmpty()) ? "0" : barcoLic.getQty())));

		if (!(LicenseUtilityNames.never.name().equalsIgnoreCase(
				barcoLic.getExpires()) || LicenseUtilityNames.perpetual.name()
				.equalsIgnoreCase(barcoLic.getExpires()))) {
			Date today = new Date();
			// Date expiryDate = getExpiryDateFromBLM(license.getProductName());
			Date expiryDate = LicenseServiceUtils.getDateFromString(
					barcoLic.getExpires(),
					LicensingConstants.LICENSE_GENERAL_DATE_FORMAT);

			if (expiryDate.before(today)) {
				license.setStatus(LicenseStatus.LicenseExpired);
			}
		}
		license.setExpiryDate(barcoLic.getExpires());

		if (license.getStartDate() == null || license.getStartDate().isEmpty()) {
			// start date format (i.e "EEE MMM d HH:mm:SS yyyy") was different
			// from
			// expiry date as it is generated by BLM library.
			String startDate = LicenseServiceUtils.getDateFromString(
					barcoLic.getStart(),
					LicensingConstants.LICENSE_BLM_START_DATE_FORMAT,
					LicensingConstants.LICENSE_GENERAL_DATE_FORMAT);
			license.setStartDate(startDate);
		}
	}

	/*
	 * Helper method to initialized all values of BarcoLicense from BLM File
	 */
	private void initializeBarcoLicenseManuallyFromBLM(String aProductName,
			BarcoLicense lic) {
		for (BLMStructure blmS : BLMStructure.values()) {
			blmS.invoke(aProductName, lic);
		}
	}

	private void updateFeature(Feature licFeature, BarcoLicense barcoLic) {
		licFeature.setName(barcoLic.getProduct());
		licFeature.setQTY(Integer.parseInt((barcoLic.getQty() == null ? "0"
				: barcoLic.getQty())));

		if (!(LicenseUtilityNames.never.name().equalsIgnoreCase(
				barcoLic.getExpires()) || LicenseUtilityNames.perpetual.name()
				.equalsIgnoreCase(barcoLic.getExpires()))) {
			Date today = new Date();
			Date expiryDate = LicenseServiceUtils.getDateFromString(
					barcoLic.getExpires(),
					LicensingConstants.LICENSE_GENERAL_DATE_FORMAT);

			if (expiryDate.before(today))
				licFeature.setStatus(LicenseStatus.LicenseExpired);
		}
		licFeature.setExpiryDate(barcoLic.getExpires());

		if (licFeature.getStartDate() == null
				|| licFeature.getStartDate().isEmpty()) {
			// start date format (i.e "EEE MMM d HH:mm:SS yyyy") was different
			// from
			// expiry date as it is generated by BLM library.
			String startDate = LicenseServiceUtils.getDateFromString(
					barcoLic.getStart(),
					LicensingConstants.LICENSE_BLM_START_DATE_FORMAT,
					LicensingConstants.LICENSE_GENERAL_DATE_FORMAT);
			licFeature.setStartDate(startDate);
		}

		// set below if needed
		// f.setFeatureVersion(barcoLic.getVersion());
	}

	private LicenseResultCodes initBLMLicense(String aProductName,
			BarcoLicense aBarcoLicense) {
		// initialize BLM for BSMD
		int blmcode = BLMSingleton.blmInstance.getBLMSingleton().LicenseSAInit(
				aProductName, license.getDivision(),
				license.getProductVersion(), UNIFY, HEART, aBarcoLicense);

		return LicenseResultCodes.translate(blmcode);
	}

	/*
	 * Initialize the meta information of the license from the CUSTOM String,
	 * stored license file.
	 * 
	 * @throws LicenseException in case the CUSTOM String is not provided in
	 * expected format in the license file
	 */
	private void processCustomKeyString(String aCustomString)
			throws LicenseException {
		String[] customs;
		String custom = aCustomString;
		// CUSTOM String not available in license file
		if (custom == null) {
			throw new LicenseException(
					"Failed to process CUSTOM key of license file. Key not found.");
		}

		// split into individual pieces of information
		custom = custom.replaceAll("\\+", " "); // replace any '+' which has
												// served as placeholder for the
												// ' ', as BLM would
		// consider a CUSTOM String containing blanks as invalid
		customs = custom.split(";");
		if (customs.length < 4) {
			mLogger.trace("Custom attribute length was lessthan four, Custom Attributes: "
					+ customs);
			throw new LicenseException(
					"Failed to process CUSTOM key of license file. "
							+ "Expected information on Customer, Project, System key and System pin. At least one of these is missing.");
		}

		// obtain individual values
		for (String customStr : customs) {
			String key;
			String value;
			String[] prop;

			prop = customStr.split("=");
			if (prop.length == 2) {
				key = prop[0];
				value = prop[1];

				if (value == null || value.trim().equals(""))
					continue;

				switch (LicenseCustomStructure.valueOf(key)) {
				case ProductID: {
					license.setProductID(value);
					break;
				}
				case Product: {
					license.setProductName(value);
					break;
				}
				case CustomerID: {
					license.setCustomerID(value);
					break;
				}
				case Customer: {
					license.setCustomerName(value);
					break;
				}
				case ProjectID: {
					license.setProjectID(value);
					break;
				}
				case Project: {
					license.setProjectName(value);
					break;
				}
				case DeviceID: {
					license.setDeviceId(value);
					break;
				}
				case LicenseKey: {
					license.setKey(value);
					break;
				}
				case StartDate: {
					license.setStartDate(value);
					break;
				}
				default: {
					throw new IllegalArgumentException(
							"Failed to process CUSTOM key of license file. Invalid format of CUSTOM key attributes Key: "
									+ key);
				}
				}
			}
		}

		// check each of the required values is available
		if (license.getLicenseType() == null
				|| license.getCustomerName() == null
				|| license.getKey() == null || license.getDeviceId() == null) {
			mLogger.warn("Custom does not have valid attribute values, Custom Attributes: "
					+ customs);
			throw new LicenseException(
					"Failed to process CUSTOM key of license file. "
							+ "Expected information on Customer name, Project name, License key and PIN code. At least one of these is missing.");
		}
	}

	/*
	 * Initialize the meta information of the feature from the CUSTOM String,
	 * stored license file.
	 */
	private void processFeatureCustomKeyString(Feature f, String aCustomString)
			throws LicenseException {
		String[] customs;
		String custom = aCustomString;

		// CUSTOM String not available in license file for this feature
		if (custom == null) {
			mLogger.info("Feature does not have any CUSTOM properties in the license file, for this feature: "
					+ f.getName());
			return;
		}

		// split into individual pieces of information
		custom = custom.replaceAll("\\+", " "); // replace any '+' which has
		// served as placeholder for the
		// ' ', as BLM would
		// consider a CUSTOM String containing blanks as invalid
		customs = custom.split(";");

		// obtain individual values
		for (String customStr : customs) {
			String key;
			String value;
			String[] prop;

			prop = customStr.split("=");
			if (prop.length == 2) {
				key = prop[0];
				value = prop[1];

				if (key == null || key.trim().equals("") || value == null
						|| value.trim().equals(""))
					continue;

				if (key.equalsIgnoreCase(FeatureCustomStructure.FeatureID
						.name())) {
					f.setID(value);
				} else if (key
						.equalsIgnoreCase(FeatureCustomStructure.StartDate
								.name())) {
					f.setStartDate(value);
				} else {
					f.getAttributesMap().put(key, value);
				}
			}
		}
	}

	private enum BLMSingleton {
		blmInstance;

		private BLM blm;
		private int blmLoadError;

		private BLMSingleton() {
			blm = new BLM();

			mLogger.debug("OSName: " + System.getProperty("os.name")
					+ "; OS.ARCH: " + System.getProperty("os.arch")
					+ "; BLM Workiing Directory: "
					+ System.getenv("BLM_DYNAMIC_DIR")
					+ "; BLM File Location: "
					+ System.getenv("BLM_LICENSE_FILE"));
			blmLoadError = blm.loadLibrary();
		}

		/**
		 * @return BLM Singleton instance
		 */
		BLM getBLMSingleton() {
			return blm;
		}

		/**
		 * @return error state if occured while loading BLM library.
		 */
		int getBLMLoadError() {
			return blmLoadError;
		}
	}

	private enum LicenseCustomStructure {
		ProductID, Product, CustomerID, Customer, ProjectID, Project, LicenseKey, DeviceID, StartDate
	}

	/**
	 * Enumeration to handle custom attributes of a Feature
	 * 
	 * @author JMRA
	 */
	private enum FeatureCustomStructure {
		FeatureID, StartDate
	}

	/*
	 * Enumeration represent attributes of a BLM data structure. And a helper to
	 * invoke license setters corresponding to BLM data.
	 * 
	 * @author JMRA
	 */
	private enum BLMStructure {
		BACKED_BY, CH, CUSTOM, DAYS, DIV, EXPIRES, FEATURE, HEART, HOSTIP, HOSTMAC, HOSTNAME, HOSTUSER, LMBACKUP, LMHOST, PORT, PRODUCT, PLUSE, QTY, REV, SERVED_BY, SITE, START, UNIFY, USER, VERSION;// KEY,
																																																		// STRING,
																																																		// PROCESS
		/*
		 * Helper method to invoke Barco license setters
		 * 
		 * @param aProductName
		 * 
		 * @param lic
		 */
		public void invoke(String aProductName, BarcoLicense lic) {
			try {
				Method m = lic.getClass().getMethod(
						"set" + getPropName(this.name()), String.class);
				String value = BLMSingleton.blmInstance.getBLMSingleton()
						.InquireKeyval(aProductName, this.name());
				m.invoke(lic, value);
			} catch (Exception e) {
				// e.printStackTrace();
			}
		}

		private String getPropName(String name) {
			name = name.split("_")[0];
			String result = "";
			if (name.equalsIgnoreCase(HOSTIP.name())) {
				int subLen = name.length() - 2;
				result = name.substring(0, 1)
						+ name.substring(1, subLen).toLowerCase()
						+ name.substring(subLen);
			} else {
				result = name.substring(0, 1) + name.substring(1).toLowerCase();
			}
			return result;
		}
	}
}