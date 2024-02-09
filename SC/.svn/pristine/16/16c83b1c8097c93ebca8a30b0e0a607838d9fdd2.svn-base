/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */
package com.barco.nimbus.license.api;

import java.io.Serializable;
import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

/**
 * License POJO - The license information is carried in this pojo. When Product
 * starts for the first time it will be initialized with trial mode of license.
 * When a new license file is uploaded the it will be updated with new license
 * information.
 * 
 * @author JMRA
 */
public class License implements Serializable {
	/**
	 * generated serial version id
	 */
	private static final long serialVersionUID = 200361141760295303L;

	@SuppressWarnings("unused")
	private enum LicenseUtilityNames {
		never, perpetual, trial, leasing, payperuse, floating
	}

	private String mLicenseType = "Nimbus";

	private String mRevision;
	private String mDivision;
	private String mProductVersion;
	private String mProductID;
	private String mProductName;
	private String mCustomerID;
	private String mCustomerName;
	private String mProjectID;
	private String mProjectName;
	private String mDeviceID;
	private String mDeviceName;

	private String mStartDate;
	private String mExpiryDate;// = LicenseUtils.getDateFromString(new Date(),
								// LicensingConstants.LICENSE_GENERAL_DATE_FORMAT);

	private int mQTY;

	// License specific
	private String mKey = "Trial";
	private String mMode = "Trial";

	private LicenseStatus mStatus = LicenseStatus.TrialLicense;

	private Set<Feature> mFeatureSet = new HashSet<Feature>();

	/**
     * 
     */
	public License() {
	}

	/**
	 * @param aRevision
	 * @param aDivision
	 * @param aLicenseType
	 * @param aProductVersion
	 */
	public License(String aRevision, String aDivision, String aLicenseType,
			String aProductVersion) {
		mRevision = aRevision;
		mDivision = aDivision;
		mLicenseType = aLicenseType;
		mProductVersion = aProductVersion;
	}

	/**
	 * @return the mLicenseType
	 */
	public String getLicenseType() {
		return mLicenseType;
	}

	/**
	 * @param aLicenseType
	 */
	public void setLicenseType(String aLicenseType) {
		mLicenseType = aLicenseType;
	}

	// /**
	// * @param mLicenseType the mLicenseType to set
	// */
	// public void setLicenseType(String mLicenseType) {
	// this.mLicenseType = mLicenseType;
	// }

	/**
	 * @return project name
	 */
	public String getProjectName() {
		return mProjectName;
	}

	/**
	 * @param aProjectName
	 */
	public void setProjectName(String aProjectName) {
		mProjectName = aProjectName;
	}

	/**
	 * @return project id
	 */
	public String getProjectID() {
		return mProjectID;
	}

	/**
	 * @param mProjectID
	 */
	public void setProjectID(String mProjectID) {
		this.mProjectID = mProjectID;
	}

	/**
	 * @return set of features belongs to license
	 */
	public Set<Feature> getFeatures() {
		return mFeatureSet;
	}

	/**
	 * @param aFeatureSet
	 */
	public void setFeatures(Set<Feature> aFeatureSet) {
		mFeatureSet = aFeatureSet;
	}

	/**
	 * @param aFeatureName
	 * @return Feature
	 */
	public Feature getFeature(String aFeatureName) {
		Feature result = null;
		for (Feature f : mFeatureSet) {
			if (f.getName().equals(aFeatureName)) {
				result = f;
				break;
			}
		}

		return result;
	}

	/**
	 * @return product name
	 */
	public String getProductName() {
		return mProductName;
	}

	/**
	 * @param mProductName
	 */
	public void setProductName(String mProductName) {
		this.mProductName = mProductName;
	}

	public String getCustomerID() {
		return mCustomerID;
	}

	public void setCustomerID(String mCustomerID) {
		this.mCustomerID = mCustomerID;
	}

	/**
	 * @return product version
	 */
	public String getProductVersion() {
		return mProductVersion;
	}

	/**
	 * @param mProductVersion
	 */
	public void setProductVersion(String mProductVersion) {
		this.mProductVersion = mProductVersion;
	}

	public String getProductID() {
		return mProductID;
	}

	public void setProductID(String mProductID) {
		this.mProductID = mProductID;
	}

	/**
	 * @return division name
	 */
	public String getDivision() {
		return mDivision;
	}

	/**
	 * @param mDivision
	 */
	public void setDivision(String mDivision) {
		this.mDivision = mDivision;
	}

	/**
	 * @return revision
	 */
	public String getRevision() {
		return mRevision;
	}

	/**
	 * @param mRevision
	 */
	public void setRevision(String mRevision) {
		this.mRevision = mRevision;
	}

	/**
	 * @return status
	 */
	public LicenseStatus getStatus() {
		return mStatus;
	}

	/**
	 * @param mLicenseStatus
	 */
	public void setStatus(LicenseStatus mLicenseStatus) {
		this.mStatus = mLicenseStatus;
	}

	/**
	 * @return quantity
	 */
	public int getQTY() {
		return mQTY;
	}

	/**
	 * @param mQTY
	 */
	public void setQTY(int mQTY) {
		this.mQTY = mQTY;
	}

	/**
	 * @return Customer Name
	 */
	public String getCustomerName() {
		return mCustomerName;
	}

	/**
	 * @param mCustomerName
	 */
	public void setCustomerName(String mCustomerName) {
		this.mCustomerName = mCustomerName;
	}

	/**
	 * @return Devide id
	 */
	public String getDeviceId() {
		return mDeviceID;
	}

	/**
	 * @param mDeviceId
	 */
	public void setDeviceId(String mDeviceId) {
		this.mDeviceID = mDeviceId;
	}

	/**
	 * @return Device name
	 */
	public String getDeviceName() {
		return mDeviceName;
	}

	/**
	 * @param mDeviceName
	 */
	public void setDeviceName(String mDeviceName) {
		this.mDeviceName = mDeviceName;
	}

	/**
	 * @return Key
	 */
	public String getKey() {
		return mKey;
	}

	/**
	 * @param mKey
	 */
	public void setKey(String mKey) {
		this.mKey = mKey;
	}

	/**
	 * @return date of expires
	 */
	public String getExpiryDate() {
		return mExpiryDate;
	}

	/**
	 * @param mExpiryDate
	 */
	public void setExpiryDate(String mExpiryDate) {
		this.mExpiryDate = mExpiryDate;
	}

	/**
	 * @return start date
	 */
	public String getStartDate() {
		return mStartDate;
	}

	/**
	 * @param mStartDate
	 */
	public void setStartDate(String mStartDate) {
		this.mStartDate = mStartDate;
	}

	/**
	 * @return license mode
	 */
	public String getMode() {
		return mMode;
	}

	/**
	 * @param mMode
	 */
	public void setMode(String mMode) {
		this.mMode = mMode;
	}

	// helper methods.
	public boolean isLicenseValid() {

		boolean isValid = Boolean.FALSE;
		if ((LicenseUtilityNames.never.name().equalsIgnoreCase(mExpiryDate) || LicenseUtilityNames.perpetual
				.name().equalsIgnoreCase(mExpiryDate))) {
			isValid = Boolean.TRUE;
		} else {
			Calendar cal = Calendar.getInstance();
			Date today = cal.getTime();
			if (!today.after(getDateFromString(mExpiryDate,
					LicensingConstants.LICENSE_GENERAL_DATE_FORMAT))) {
				isValid = Boolean.TRUE;
			}
		}

		return isValid;
	}

	private Date getDateFromString(String aDate, String aDateFormat) {
		Date dateFromBLM = null;
		if (aDateFormat == null)
			aDateFormat = LicensingConstants.LICENSE_GENERAL_DATE_FORMAT;

		DateFormat formatter = new SimpleDateFormat(aDateFormat, Locale.US);
		try {
			dateFromBLM = formatter.parse(aDate);
		} catch (ParseException e) {
		}
		return dateFromBLM;
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append(this.getClass().getSimpleName() + "[");
		sb.append("LicenseType=").append(mLicenseType);
		sb.append("; Rivision=").append(mRevision);
		sb.append("; Division=").append(mDivision);
		sb.append("; ProductID=").append(mProductID);
		sb.append("; Product=").append(mProductName);
		sb.append("; ProcuctVersion=").append(mProductVersion);
		sb.append("; CustomerID=").append(mCustomerID);
		sb.append("; CustomerName=").append(mCustomerName);
		sb.append("; ProjectID=").append(mProjectID);
		sb.append("; Project=").append(mProjectName);
		sb.append("; LicenseKey=").append(mKey);
		sb.append("; DeviceID=").append(mDeviceID);
		sb.append("; StartDate=").append(mStartDate);
		sb.append("; ExpiryDate=").append(mExpiryDate);
		sb.append("; QTY=").append(mQTY);
		sb.append("; LicenseStatus=").append(mStatus);
		sb.append("; FetureSet=").append(mFeatureSet);
		sb.append("]");
		return sb.toString();
	}
}
