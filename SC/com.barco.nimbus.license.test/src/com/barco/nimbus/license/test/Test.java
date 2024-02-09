package com.barco.nimbus.license.test;

import java.io.File;
import java.security.AlgorithmParameters;
import java.security.spec.KeySpec;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;
import java.util.concurrent.TimeUnit;

import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.IvParameterSpec;
import javax.crypto.spec.PBEKeySpec;
import javax.crypto.spec.SecretKeySpec;

import com.barco.nimbus.license.LicenseAdaptor;
import com.barco.nimbus.license.LicenseService;
import com.barco.nimbus.license.api.License;

public class Test {
	Cipher dcipher;

	byte[] salt = new String("12345678").getBytes();
	int iterationCount = 1024;
	int keyStrength = 256;
	SecretKey key;
	byte[] iv;

	Test(String passPhrase) throws Exception {
		SecretKeyFactory factory = SecretKeyFactory
				.getInstance("PBKDF2WithHmacSHA1");
		KeySpec spec = new PBEKeySpec(passPhrase.toCharArray(), salt,
				iterationCount, keyStrength);
		SecretKey tmp = factory.generateSecret(spec);
		key = new SecretKeySpec(tmp.getEncoded(), "AES");
		dcipher = Cipher.getInstance("AES/CBC/PKCS5Padding");
	}

	public String encrypt(String data) throws Exception {
		dcipher.init(Cipher.ENCRYPT_MODE, key);
		AlgorithmParameters params = dcipher.getParameters();
		iv = params.getParameterSpec(IvParameterSpec.class).getIV();
		byte[] utf8EncryptedData = dcipher.doFinal(data.getBytes());
//		String base64EncryptedData = new sun.misc.BASE64Encoder()
//				.encodeBuffer(utf8EncryptedData);
//
//		System.out.println("IV "
//				+ new sun.misc.BASE64Encoder().encodeBuffer(iv));
//		System.out.println("Encrypted Data " + base64EncryptedData);
		return "";//base64EncryptedData;
	}

	public String decrypt(String base64EncryptedData) throws Exception {
		dcipher.init(Cipher.DECRYPT_MODE, key, new IvParameterSpec(iv));
//		byte[] decryptedData = new sun.misc.BASE64Decoder()
//				.decodeBuffer(base64EncryptedData);
//		byte[] utf8 = dcipher.doFinal(decryptedData);
		return "";//new String(utf8, "UTF8");
	}

	public static void main(String args[]) throws Exception {
		//Decrypter decrypter = new Decrypter("ABCDEFGHIJKL");
//		String encrypted = decrypter
//				.encrypt("the quick brown fox jumps over the lazy dog");
//		String decrypted = decrypter.decrypt(encrypted);
//		System.out.println(decrypted);
//		Calendar cal = Calendar.getInstance();
//		cal.setTime(new Date());
//		cal.add(Calendar.YEAR, 2);
//		cal.add(Calendar.MONTH,  3);
//		cal.add(Calendar.DATE,  23);
//		System.out.println(cal.getTime());
		
		// license read test
//		License licenseBeforeUploadNew = new License("4", "BSMD", "Nimbus",
//				"4.1");
//		LicenseAdaptor blmAgent = new LicenseAdaptor();
//
//		System.out.println(blmAgent.readLicense(licenseBeforeUploadNew));
		
		System.out.println( new File("resources/License.xml").toURI().toString());
		
		
		
	}
	
	private int getRemainingDays(Date expiry) {
		// int days = (int)( (expiry.getTime() - today.getTime()) / (1000 * 60 *
		// 60 * 24));
		Calendar cal = Calendar.getInstance();
		Date today = cal.getTime();
		long diffBetweenDates = expiry.getTime() - today.getTime();

		DateFormat df = new SimpleDateFormat("yyyy : DDD : HH : mm : ss");

		df.setTimeZone(TimeZone.getDefault());
		String f = df.format(diffBetweenDates);
		System.out.println("-----------: " + f);

		System.out.println("Today : " + cal.getTime());

		// Add TRIAL_PERIOD_DAYS days to the calendar
		cal.add(Calendar.DATE, 0);
		System.out.println(0 + " days ago: " + cal.getTime());

		// Interval interval =
		// new Interval(startDate.getTime(), endDate.getTime());
		// Period period = interval.toPeriod();
		//
		// System.out.printf(
		// "%d years, %d months, %d days, %d hours, %d minutes, %d seconds%n",
		// period.getYears(), period.getMonths(), period.getDays(),
		// period.getHours(), period.getMinutes(), period.getSeconds());

		return (int) TimeUnit.DAYS.convert(diffBetweenDates,
				TimeUnit.MILLISECONDS);
	}
}