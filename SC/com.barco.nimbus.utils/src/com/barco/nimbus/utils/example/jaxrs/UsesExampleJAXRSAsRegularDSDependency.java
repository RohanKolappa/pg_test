package com.barco.nimbus.utils.example.jaxrs;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

@Component
public class UsesExampleJAXRSAsRegularDSDependency {
	private IExampleJAXRSService items;

	@Reference
	public void bind(IExampleJAXRSService x) {
		this.items = x;
	}

	public void unbind(IExampleJAXRSService x) {
	}

	@Activate
	public void activate() {
		Logger.getLogger(UsesExampleJAXRSAsRegularDSDependency.class).info(
				"got " + items.getAll()[0].owner);
	}

}
