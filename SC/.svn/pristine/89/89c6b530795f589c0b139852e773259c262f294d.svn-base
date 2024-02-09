package com.barco.nimbus.displaymanagement.bms;

import java.io.IOException;
import java.net.URL;
import java.util.concurrent.atomic.AtomicReference;

import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;
import org.osgi.service.http.HttpContext;
import org.osgi.service.http.HttpService;
import org.osgi.service.http.NamespaceException;

@Component
public class AtBootHostHTTPResources {

	final AtomicReference<HttpService> ref = new AtomicReference<>();
	final HttpContext context = new HttpContext() {
		public boolean handleSecurity(HttpServletRequest request, HttpServletResponse response) throws IOException {
			return true;
		}

		public URL getResource(String name) {
			Logger.getLogger(AtBootHostHTTPResources.class).info(name);
			return getClass().getResource(name);
		}

		public String getMimeType(String name) {
			return null;
		}
	};

	@Reference
	public void setHTTP(HttpService http) {
		ref.set(http);
	}

	public void unsetHTTP(HttpService http) {
		ref.compareAndSet(http, null);
	}

	@Activate
	public void start() throws NamespaceException {
//		ref.get().registerResources("/barco-webservice", "/www", context);
//		ref.get().registerResources("/barco-webservice/includes", "/www/includes", context);
		ref.get().registerResources("/barco-webservice", "/com/barco/compose/display/session", context);
	}
}
