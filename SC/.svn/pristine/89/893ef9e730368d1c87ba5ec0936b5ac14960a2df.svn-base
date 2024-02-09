package com.barco.nimbus.utils.example.dom4j;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Reference;

/**
 * Example of a service that depends on another service.
 * The @Reference method will be called as soon as the dependency is available.
 * When all dependencies are met, the @Activate will be called.
 */
@Component
public class Example {
	
	private IDOM4J dom4j;

	@Reference
	public void bind(IDOM4J dom4j) {
		this.dom4j = dom4j;
	}
	public void unbind(IDOM4J dom4j) {
		this.dom4j = null;
	}
	
	
	@Activate
	public void start() throws Exception {
		Document doc = dom4j.stringToDocument("<root/>");
		Logger.getLogger(Example.class).info(doc + ": " + doc.asXML());
	}
}
