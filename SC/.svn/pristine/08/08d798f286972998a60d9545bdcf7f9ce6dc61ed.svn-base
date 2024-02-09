/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license.api.util;

import java.text.DateFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.Locale;
import java.util.concurrent.TimeUnit;

import com.barco.nimbus.license.api.LicensingConstants;

/**
 * @author JMRA
 * 
 */
public class LicenseUtils {

	private static Date getDateFromString(String aDate, String aDateFormat) {

		// TODO: handle aDate string with never value.
		Date dateFromBLM = null;
		if (aDateFormat == null)
			aDateFormat = LicensingConstants.LICENSE_GENERAL_DATE_FORMAT;

		DateFormat formatter = new SimpleDateFormat(aDateFormat, Locale.US);
		try {
			dateFromBLM = formatter.parse(aDate);
		} catch (ParseException e) {
			// mLogger.warn("Date format exception", e);
		}
		return dateFromBLM;
	}

	public static int getRemainingDaysFromExpiry(String dateExpiryString) {

		Date expiry = getDateFromString(dateExpiryString,
				LicensingConstants.LICENSE_GENERAL_DATE_FORMAT);
		Calendar cal = Calendar.getInstance();
		Date today = cal.getTime();
		long diffBetweenDates = expiry.getTime() - today.getTime();

		return (int) TimeUnit.DAYS.convert(diffBetweenDates,
				TimeUnit.MILLISECONDS);
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
}
