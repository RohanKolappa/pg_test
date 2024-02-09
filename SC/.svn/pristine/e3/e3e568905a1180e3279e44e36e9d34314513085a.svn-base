package com.barco.nimbus.license;

import javax.servlet.ServletException;

import org.apache.log4j.Logger;
import org.glassfish.jersey.media.multipart.MultiPartFeature;
import org.glassfish.jersey.server.ResourceConfig;
import org.glassfish.jersey.servlet.ServletContainer;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.http.HttpContext;
import org.osgi.service.http.HttpService;
import org.osgi.service.http.NamespaceException;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.barco.nimbus.license.api.LicensingConstants;

@SuppressWarnings("rawtypes")
public class LicenseJAXRSServletTracker extends ServiceTracker {
	private static Logger mLogger = Logger
			.getLogger(LicenseJAXRSServletTracker.class);

	private HttpService httpService = null;

	@SuppressWarnings("unchecked")
	public LicenseJAXRSServletTracker(BundleContext context, String service,
			ServiceTrackerCustomizer customizer) {
		super(context, service, customizer);
	}

	@SuppressWarnings("unchecked")
	@Override
	public Object addingService(ServiceReference serviceRef) {
		httpService = (HttpService) super.addingService(serviceRef);
		registerServlets();
		return httpService;
	}

	@SuppressWarnings("unchecked")
	@Override
	public void removedService(ServiceReference ref, Object service) {
		if (httpService == service) {
			unregisterServlets();
			httpService = null;
		}
		super.removedService(ref, service);
	}

	private void registerServlets() {
		mLogger.debug("registerServlets: License HTTP service registering servlets");
		try {
			HttpContext httpContext = httpService.createDefaultHttpContext();
			httpService
					.registerServlet(
							LicensingConstants.LICENSE_HTTP_SERVICE_CONTEXT_NAME,
							new ServletContainer(new ResourceConfig(
									LicenseJAXRSServlet.class,
									MultiPartFeature.class)), null, httpContext);
		} catch (ServletException se) {
			// TODO: throw new RuntimeException(se);
			mLogger.warn("registerServlets: ServletException - ", se);
		} catch (NamespaceException se) {
			// TODO: throw new RuntimeException(se);
			mLogger.warn("registerServlets: NamespaceException - ", se);
		}
		mLogger.debug("registerServlets: License HTTP service servlets registered");
	}

	private void unregisterServlets() {
		if (this.httpService != null) {
			mLogger.debug("unregisterServlets: License HTTP service unregistering servlets");
			httpService
					.unregister(LicensingConstants.LICENSE_HTTP_SERVICE_CONTEXT_NAME);
			mLogger.debug("unregisterServlets: License HTTP service servlets unregistered");
		}
	}
}
