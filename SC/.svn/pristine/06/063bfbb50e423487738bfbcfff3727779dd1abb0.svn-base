/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.osgi.framework.BundleContext;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;
import org.osgi.service.http.HttpService;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.license.api.License;
import com.barco.nimbus.license.api.LicenseException;
import com.barco.nimbus.license.api.LicenseServiceCallbackI;
import com.barco.nimbus.license.api.LicenseServiceI;
import com.barco.nimbus.license.api.LicenseStatus;
import com.barco.nimbus.license.api.LicensingConstants;
import com.barco.nimbus.license.util.FileUtils;
import com.barco.nimbus.license.util.LicenseServiceUtils;

/**
 * @author JMRA
 */
@Component
public class LicenseService implements LicenseServiceI,
		LicenseSchedulerCallbackI {
	private static final String LICENSE_OUTPUT_FILE = System
			.getenv("BLM_LICENSE_FILE");

	private static final Logger logger = Logger.getLogger(LicenseService.class);
	private static final int TRIAL_PERIOD_DAYS = 30;
	private static final String collectionName = "ipvs.default.licenselist";

	enum LicenseUtilityNames {
		never, perpetual, trial, leasing, payperuse, floating
	}

	private List<LicenseServiceCallbackI> mNotificationListeners = new ArrayList<LicenseServiceCallbackI>();
	private License licenseDetails;
	private BundleContext bundleContext;
	private LicenseJAXRSServletTracker licenseTracker;
	private DBI db;

	@Reference
	public void setDb(DBI db) {
		logger.info(":setDb");
		this.db = db;
		logger.info(":setDb, " + this.db);
	}

	public void unsetDb(DBI db) {
		this.db = null;
		logger.info(":unsetDb, " + this.db);
	}

	/**
	 * @throws Exception
	 */
	@Activate
	public void start(BundleContext context) throws Exception {
		bundleContext = context;

		initialize();

		licenseTracker = new LicenseJAXRSServletTracker(bundleContext,
				HttpService.class.getName(), null);

		licenseTracker.open();

		logger.info(":start; license service started.");
	}

	/**
	 * @throws Exception
	 */
	@Deactivate
	public void stop() throws Exception {
		logger.info(":stop; Stoping license service...");
		// clean all schedulers
		LicenseScheduler.LicenseSchedulerSingleton.instance
				.getLicenseScheduler().clear();
	}

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceI#put()
	 */
	@Override
	public void put() throws LicenseException {
		logger.info(":put; License upload started... ");
		if (!new File(LICENSE_OUTPUT_FILE).exists()) {
			throw new LicenseException(LICENSE_FILENOTFOUND_CODE,
					"License file location doesn't exit at: "
							+ LICENSE_OUTPUT_FILE);
		}
		// safe initialization to handle load license, as BLM module needs
		// revision, division, product and version to read content from *.lic
		// file.
		License licenseBeforeUploadNew = new License("4", "BDEF", "Nimbus",
				"4.1");
		Document dbLicDocument = null;
		try {
			// get license from DB
			dbLicDocument = getLicenseDocumentFromDB();

			if (dbLicDocument != null && dbLicDocument.hasContent())
				licenseBeforeUploadNew = LicenseServiceUtils
						.getLicense(dbLicDocument);

			LicenseAdaptor blmAgent = new LicenseAdaptor();
			licenseDetails = blmAgent.readLicense(licenseBeforeUploadNew);

			// add license to database
			addLicenseToDB(dbLicDocument);
			// after license written to DB, terminate license from BLM, as
			// license service read license from DB.
			blmAgent.terminate(licenseDetails);

		} catch (LicenseException le) {
			licenseDetails.setStatus(LicenseStatus.LicenseInvalid);
			logger.error(
					":put; Invalid license uploaded, Please upload correct license file. ",
					le);
			throw new LicenseException(LICENSE_INVALID_CODE, le.getMessage());
		} catch (Exception e) {
			logger.error(
					":put; Exception occured while reading license information. ",
					e);
			throw new LicenseException(LICENSE_INVALID_CODE, e.getMessage(), e);
		}

		if (licenseDetails.getStatus() != LicenseStatus.TrialLicense) {
			startLicenseScheduler();
			notifyLicenseUpdate(licenseDetails.getStatus());
		}

		logger.info(":put; License uploaded successfully. ");
	}

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceI#put(java.lang.String)
	 */
	@Override
	public void put(String anInputFileLoacation) throws LicenseException {
		if (anInputFileLoacation == null) {
			throw new LicenseException(
					"Invalid license file location, location can't be null");
		}

		try {
			InputStream fis = new FileInputStream(
					new File(anInputFileLoacation));
			FileUtils.writeToLicenseFile(fis);

		} catch (FileNotFoundException e) {
			throw new LicenseException(
					"License file doesn't exit at given location.", e);
		}

		put();
	}

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceI#put(java.io.InputStream)
	 */
	@Override
	public void put(InputStream aLicenseInputStream) throws LicenseException {
		if (aLicenseInputStream == null) {
			throw new LicenseException("Invalid input stream");
		}

		FileUtils.writeToLicenseFile(aLicenseInputStream);
		put();
	}

	/**
	 * @see com.barco.nimbus.license.api.LicenseAPII#get()
	 */
	@Override
	public License get() {
		return licenseDetails;
	}

	/**
	 * @see com.barco.nimbus.license.LicenseSchedulerCallbackI#notifyLicenseUpdate(com.barco.nimbus.license.api.LicenseStatus)
	 */
	@Override
	public void notifyLicenseUpdate(LicenseStatus aLicenseStatus) {
		licenseDetails.setStatus(aLicenseStatus);

		synchronized (mNotificationListeners) {
			for (LicenseServiceCallbackI listener : mNotificationListeners) {
				listener.handleLicenseUpdated(get());
			}
		}
	}

	/**
	 * Initialize license at startup
	 * 
	 * @throws Exception
	 */
	private void initialize() throws Exception {
		try {
			Document xmlDocument = getLicenseDocumentFromDB();

			// initialize license from DB
			if (xmlDocument != null && xmlDocument.hasContent())
				licenseDetails = LicenseServiceUtils.getLicense(xmlDocument);
			Calendar cal = Calendar.getInstance();
			Date today = cal.getTime();
			if (licenseDetails == null) {

				// initialize license form Trial license file
				FileUtils.copyLicenseFile("TrialLicense.lic");
				put();
				// setting trial days to expiration of Trial License
				cal.add(Calendar.DATE, TRIAL_PERIOD_DAYS);
				Date expiry = cal.getTime();
				licenseDetails
						.setStartDate(LicenseServiceUtils.getDateFromString(
								today,
								LicensingConstants.LICENSE_GENERAL_DATE_FORMAT));

				licenseDetails
						.setExpiryDate(LicenseServiceUtils.getDateFromString(
								expiry,
								LicensingConstants.LICENSE_GENERAL_DATE_FORMAT));

				Document doc = getLicenseDocumentFromDB();
				if (doc != null) {
					String nid = LicenseServiceUtils.getLicenseNID(doc);
					String validityXpath = "//"
							+ LicensingConstants.LICENSE_DATA + "/"
							+ LicensingConstants.LICENSE + "/"
							+ LicensingConstants.VALIDITY;
					String startXpath = validityXpath + "/"
							+ LicensingConstants.START_DATE;
					String expiresXpath = validityXpath + "/"
							+ LicensingConstants.EXPIRY_DATE;
					db.replaceAttribute(collectionName, nid, startXpath, null,
							LicenseServiceUtils
									.getEncryptedString(licenseDetails
											.getStartDate()));
					db.replaceAttribute(collectionName, nid, expiresXpath,
							null, LicenseServiceUtils
									.getEncryptedString(licenseDetails
											.getExpiryDate()));
				}
				logger.info(
						":initialize; License initialized with trial. License Details: ",
						licenseDetails);
			} else {
				if (!(LicenseUtilityNames.never.name().equalsIgnoreCase(
						licenseDetails.getExpiryDate()) || LicenseUtilityNames.perpetual
						.name()
						.equalsIgnoreCase(licenseDetails.getExpiryDate()))
						&& today.after(LicenseServiceUtils.getDateFromString(
								licenseDetails.getExpiryDate(),
								LicensingConstants.LICENSE_GENERAL_DATE_FORMAT))) {
					licenseDetails.setStatus(LicenseStatus.LicenseExpired);
				}
			}
		} catch (Exception e) {
			logger.error(":initialize; Faild to initialize trial license.", e);
		} finally {
			FileUtils.cleanupLicenseResources();
		}

		startLicenseScheduler();
	}

	/*
	 * Helper to read license document from Database
	 */
	private Document getLicenseDocumentFromDB() {
		Document doc = null;
		String xquery = "for $doc in collection('" + collectionName
				+ "') return $doc";
		try {
			List<Document> licDocList = db.readQueryDocumentList(xquery);
			if (licDocList != null && !licDocList.isEmpty())
				doc = licDocList.get(0);
		} catch (Exception e) {
			logger.trace(
					"Error occured while reading license document from Database",
					e);
		}

		return doc;
	}

	/*
	 * Utility method to add XML document to database and ensure that there will
	 * be only one license XML document in the Database
	 */
	private void addLicenseToDB(Document aDbLicDocument) {
		try {
			Document newLicenseDoc = LicenseServiceUtils
					.getLicenseXMLDocument(licenseDetails);
			if (aDbLicDocument == null) {
				db.addDocument(collectionName,
						db.createNIDFromCollectionName(collectionName),
						newLicenseDoc);
			} else {
				String nid = LicenseServiceUtils.getLicenseNID(aDbLicDocument);
				db.replaceDocument(collectionName, nid, null, newLicenseDoc);
			}
		} catch (Exception e) {
			logger.error("Exception occured while writing license to DB", e);
		}
	}

	private void startLicenseScheduler() {
		//TODO: Check & Clear all or necessary Schedulers
		LicenseScheduler.LicenseSchedulerSingleton.instance
				.getLicenseScheduler().clear();

		// TODO: scheduler for License, but ensure only one scheduler task runs
		// for license key at given point of time.
		String expiry = licenseDetails.getExpiryDate();
		LicenseStatus status = licenseDetails.getStatus();

		if (!(LicenseUtilityNames.never.name().equalsIgnoreCase(expiry) || LicenseUtilityNames.perpetual
				.name().equalsIgnoreCase(expiry))
				&& (status == LicenseStatus.LicenseAccepted || status == LicenseStatus.TrialLicense)) {
			LicenseScheduler scheduler = LicenseScheduler.LicenseSchedulerSingleton.instance
					.getLicenseScheduler();

			if (scheduler.getLicenseSchedulerCallback() == null)
				scheduler.setmLicenseSchedulerCallback(this);
			scheduler.schedule(LicenseServiceUtils.getDateFromString(
					licenseDetails.getExpiryDate(),
					LicensingConstants.LICENSE_GENERAL_DATE_FORMAT));
		}
	}

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceI#registerCallback(com.barco.nimbus.license.api.LicenseServiceCallbackI)
	 */
	@Override
	public void registerCallback(LicenseServiceCallbackI licenseServiceCallbackI) {
		synchronized (mNotificationListeners) {
			mNotificationListeners.add(licenseServiceCallbackI);
		}
	}
}
