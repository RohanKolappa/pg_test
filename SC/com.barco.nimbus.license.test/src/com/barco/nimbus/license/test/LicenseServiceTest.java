package com.barco.nimbus.license.test;

import static org.junit.Assert.assertEquals;
import static org.mockito.Mockito.mock;

import java.io.InputStream;

import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.mockito.InjectMocks;
import org.mockito.runners.MockitoJUnitRunner;
import org.osgi.framework.BundleContext;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.license.LicenseService;
import com.barco.nimbus.license.api.License;
import com.barco.nimbus.license.api.LicenseStatus;

@RunWith(MockitoJUnitRunner.class)
public class LicenseServiceTest {

	@InjectMocks
	LicenseService licService;

	@BeforeClass
	public static void beforeClassInitialization()
	{
		try {
			System.loadLibrary("gnu_regex");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	@Before
	public void before() throws Exception {

		final BundleContext bc = mock(BundleContext.class);
		DBI db = mock(DBI.class);
		licService.setDb(db);

		licService.start(bc);
	}

	@Test
	public void testTrialLicense() {
		License lic = licService.get();
		assertEquals(LicenseStatus.TrialLicense, lic.getStatus());
	}

	@Test
	public void testLicensePutAndVerifyValidLicense() {
		try {
			InputStream is = LicenseServiceTest.class
					.getResourceAsStream("LicenseExpiresOn31-Dec-2014.lic");
			licService.put(is);
			is.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		License lic = licService.get();
		assertEquals(LicenseStatus.LicenseAccepted, lic.getStatus());
	}

	@Test
	public void testLicensePutAndVerifyInvalidLicense() {
		try {
			InputStream is = LicenseServiceTest.class
					.getResourceAsStream("InvalidLicense.lic");
			licService.put(is);
			is.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		License lic = licService.get();
		assertEquals(LicenseStatus.LicenseInvalid, lic.getStatus());
	}

	@Test
	public void testLicensePutAndVerifyExpiredLicense() {
		try {
			InputStream is = LicenseServiceTest.class
					.getResourceAsStream("ExpiredLicense.lic");
			licService.put(is);
			is.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
		License lic = licService.get();
		assertEquals(LicenseStatus.LicenseExpired, lic.getStatus());
	}
}
