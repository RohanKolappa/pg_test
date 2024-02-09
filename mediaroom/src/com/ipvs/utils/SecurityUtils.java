package com.ipvs.utils;

import java.security.SecureRandom;
import java.security.Security;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import com.sun.net.ssl.internal.ssl.Provider;

public class SecurityUtils {

	/**
	 * Returns the SSL connection factory to trust server SSL certificates for
	 * HTTPS URL.
	 */
	public static SSLSocketFactory getTrustServerConnectionFactory()
			throws Exception {
		Security.addProvider(new Provider());
		TrustManager[] trustAllSSLCertificates = new TrustManager[] { new X509TrustManager() {
			@Override
			public X509Certificate[] getAcceptedIssuers() {
				return null;
			}

			@Override
			public void checkServerTrusted(X509Certificate[] certs,
					String authType) throws CertificateException {
			}

			@Override
			public void checkClientTrusted(X509Certificate[] certs,
					String authType) throws CertificateException {
			}
		} };

		SSLContext sc = SSLContext.getInstance("SSL");
		sc.init(null, trustAllSSLCertificates, new SecureRandom());
		// HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());
		// HttpsURLConnection.setDefaultHostnameVerifier(hv);

		return sc.getSocketFactory();
	}

	public static HostnameVerifier getHostVerifier() {

		HostnameVerifier hv = new HostnameVerifier() {
			@Override
			public boolean verify(String urlHostName, SSLSession session) {
				return true;
			}
		};
		return hv;
	}

}
