/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license.api;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Map;

/**
 * @author JMRA
 */
public class Feature implements Serializable {
	/**
	 * generated serial version id
	 */
	private static final long serialVersionUID = -81753811076520631L;

	private String mID;
	private String mName;
	private String mStartDate;
	private String mExpiryDate;
	private int mQTY;

	private LicenseStatus mStatus = LicenseStatus.LicenseAccepted;

	private Map<String, String> mAttributesMap = new HashMap<String, String>();

	public String getID() {
		return mID;
	}

	public void setID(String mID) {
		this.mID = mID;
	}

	/**
	 * @return name of license feature
	 */
	public String getName() {
		return mName;
	}

	/**
	 * @param mName
	 */
	public void setName(String mName) {
		this.mName = mName;
	}

	/**
	 * @return status
	 */
	public LicenseStatus getStatus() {
		return mStatus;
	}

	/**
	 * @param mStatus
	 */
	public void setStatus(LicenseStatus mStatus) {
		this.mStatus = mStatus;
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
	 * @return date of expiration
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

	public String getStartDate() {
		return mStartDate;
	}

	public void setStartDate(String mStartDate) {
		this.mStartDate = mStartDate;
	}

	/**
	 * @return the mAttributesMap
	 */
	public Map<String, String> getAttributesMap() {
		return mAttributesMap;
	}

	/**
	 * @param mAttributesMap
	 *            the mAttributesMap to set
	 */
	public void setAttributesMap(Map<String, String> mAttributesMap) {
		this.mAttributesMap = mAttributesMap;
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append(this.getClass().getSimpleName() + "[");
		sb.append("FeatureID=").append(mID);
		sb.append("; FeatureName=").append(mName);
		sb.append("; QTY=").append(mQTY);
		sb.append("; StartDate=").append(mStartDate);
		sb.append("; ExpiryDate=").append(mExpiryDate);
		sb.append("; FeatureLicenseStatus=").append(mStatus);
		sb.append("; AttributesMap=").append(mAttributesMap);
		sb.append("]");
		return sb.toString();
	}

}
