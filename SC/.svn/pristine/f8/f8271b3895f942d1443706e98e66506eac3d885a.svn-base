package com.barco.nimbus.license;

import java.io.IOException;
import java.io.InputStream;

import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.Response.ResponseBuilder;

import org.apache.log4j.Logger;
import org.glassfish.jersey.media.multipart.FormDataMultiPart;
import org.json.JSONObject;
import org.json.XML;
import org.osgi.framework.BundleContext;
import org.osgi.framework.FrameworkUtil;
import org.osgi.framework.ServiceReference;

import com.barco.nimbus.license.LicenseService;
import com.barco.nimbus.license.api.License;
import com.barco.nimbus.license.api.LicenseServiceI;
import com.barco.nimbus.license.api.LicensingConstants;
import com.barco.nimbus.license.util.LicenseServiceUtils;

public class LicenseJAXRSServlet implements LicenseJAXRSI {

	private static Logger mLogger = Logger.getLogger(LicenseJAXRSServlet.class);

	@Override
	public Response get() {

		License lic = getLicenseService().get();

		ResponseBuilder jaxrs = lic == null ? Response.noContent() : Response
				.ok(lic).type(MediaType.APPLICATION_JSON);

		JSONObject xmlJSONObj = XML.toJSONObject(LicenseServiceUtils
				.getLicenseXMLDocumentForUser(lic).asXML());

		// jaxrs.entity(LicenseServiceUtils.getLicenseXMLDocumentForUser(lic).asXML());
		System.out.println("==========================================: "
				+ xmlJSONObj.toString());
		jaxrs.entity(xmlJSONObj.toString());

		return jaxrs.build();
	}

	@Override
	public Response put(FormDataMultiPart multiPartFile) {
		return loadLicense(multiPartFile.getField(
				LicensingConstants.LICENSE_FORM_FIELD_NAME).getValueAs(
				InputStream.class));
	}

	private Response loadLicense(InputStream fileStream) {
		String responseString = "";
		try {
			if (fileStream == null)
				responseString = "Invalid license file uploaded. Please try with correct license file.";
			else {
				getLicenseService().put(fileStream);
			}
		} catch (Exception e) {
			responseString = "ERROR: Invalid license file uploaded."
					+ e.getMessage();
		} finally {
			if (fileStream != null)
				try {
					fileStream.close();
				} catch (IOException e) {
					// skip
				}
		}
		if (responseString.equals("")) {
			responseString = "License uploaded sucessfully";
		}
		return Response.status(200).entity(responseString).build();
	}

	private LicenseServiceI getLicenseService() {

		mLogger.info(LicenseJAXRSServlet.class.getName()
				+ "getLicenseService:; getting license service.");
		BundleContext bundleContext = FrameworkUtil.getBundle(
				LicenseService.class).getBundleContext();
		ServiceReference<?> reference = bundleContext
				.getServiceReference(LicenseServiceI.class.getName());
		mLogger.info(LicenseJAXRSServlet.class.getName()
				+ "getLicenseService:; got license service ref - Bundle ID: "
				+ reference.getBundle().getBundleId());

		return (LicenseServiceI) bundleContext.getService(reference);
	}
}
