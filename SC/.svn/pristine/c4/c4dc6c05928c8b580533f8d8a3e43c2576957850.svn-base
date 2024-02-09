package com.barco.nimbus.displaymanagement.bms;

import javax.ws.rs.ext.MessageBodyReader;
import javax.ws.rs.ext.MessageBodyWriter;

import org.osgi.framework.BundleContext;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;

import com.barco.compose.util.jaxrs.ByteObjectBodyReader;
import com.barco.compose.util.jaxrs.ByteObjectBodyWriter;

@Component
public class AtBootRegisterJAXRSProviders {

	@Activate
	public void start(BundleContext x) {
		x.registerService(MessageBodyWriter.class, new ByteObjectBodyWriter(), null);
		x.registerService(MessageBodyReader.class, new ByteObjectBodyReader(), null);
	}
}
