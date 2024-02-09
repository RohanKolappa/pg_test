package com.barco.nimbus.license.util;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import com.barco.nimbus.license.api.LicenseException;

/**
 * @author JMRA
 */
public class FileUtils {
	private static String mLicenseFilePath = System.getenv("BLM_LICENSE_FILE") == null ? ""
			: System.getenv("BLM_LICENSE_FILE");

	private static final int DEFAULT_BUFFER_SIZE = 1024;

	/**
	 * @param anInputFileName
	 */
	public static void copyLicenseFile(String anInputFileName) {

		InputStream in = FileUtils.class.getResourceAsStream(anInputFileName);

		writeToLicenseFile(in);
	}

	/**
	 * @param anLicenseFileInputStream
	 */
	public static void writeToLicenseFile(InputStream anLicenseFileInputStream) {
		BufferedOutputStream writeToLicenseFile = null;
		byte[] buffer = new byte[DEFAULT_BUFFER_SIZE];

		try {
			if (mLicenseFilePath.isEmpty()) {
				throw new LicenseException("Environment variable BLM_LICENSE_FILE is not set properly .");
			}
			writeToLicenseFile = new BufferedOutputStream(new FileOutputStream(new File(mLicenseFilePath)));
			int n = 0;
			while (-1 != (n = anLicenseFileInputStream.read(buffer))) {
				writeToLicenseFile.write(buffer, 0, n);
			}
			writeToLicenseFile.flush();
		} catch (FileNotFoundException e) {
			throw new LicenseException(
					"License file doesn't exit at given location.", e);
		} catch (IOException e) {
			throw new LicenseException(
					"IO Exception while reading license file content.", e);
		} finally {
			if (anLicenseFileInputStream != null) {
				try {

					anLicenseFileInputStream.close();
				} catch (IOException e) {

				}
			}

			if (writeToLicenseFile != null) {
				try {
					writeToLicenseFile.close();
				} catch (IOException e) {

				}
			}
		}
	}

	/**
	 * Clean up all license related files
	 */
	public static void cleanupLicenseResources() {
		File lic = new File(mLicenseFilePath);
		if (lic.exists()) {
			lic.delete();
		}
	}
}