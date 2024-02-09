package com.barco.nimbus.utils.example.jaxrs;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Component;

/**
 * GET http://localhost:9090/services/examples/todo gives
 * [{"owner":1,"id":2,"description":null},{"owner":1,"id":3,"description":null}]
 * while in the logging we see [Component Resolve Thread (Bundle 111)] INFO
 * com.barco.nimbus.utils.example.jaxrs.UsesExampleJAXRSAsRegularDSDependency -
 * got 1
 * <p/>
 * The example demonstrates that if you annotate a component with both @Component
 * and @Path (see IExampleJAXRSService), only one instance will be created, that
 * serves both DS clients as well as JAXRS clients. In other words, the JAXRS
 * handling is not creating separate instances, but using the DS service
 * instances.
 */
@Component
public class ExampleJAXRSService implements IExampleJAXRSService {

	private static int instanceGen;

	final int instance;
	{
		instance = ++instanceGen;
		// this will only print once: only one instance is created
		Logger.getLogger(ExampleJAXRSService.class).info("created instance " + instance);
	}

	@Override
	public Item[] getAll() {
		return new Item[] { new Item(instance), new Item(instance) };
	}
}
