package com.barco.nimbus.utils.example.dom4j;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.osgi.service.component.annotations.Component;

/**
 * Example of a service without dependent services.
 * The @Component will trigger DS to publish this service.
 */
@Component
public class DOM4JComponent implements IDOM4J{
	
	public Document stringToDocument(String s) throws Exception {
		Logger.getLogger(DOM4JComponent.class).info(s);
		return DocumentHelper.parseText(s);
	}

}
