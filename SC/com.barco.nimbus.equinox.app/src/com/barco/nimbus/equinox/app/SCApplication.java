package com.barco.nimbus.equinox.app;

import org.eclipse.equinox.app.IApplication;
import org.eclipse.equinox.app.IApplicationContext;

public class SCApplication implements IApplication {

	@Override
	public Object start(IApplicationContext context) throws Exception {
		System.out.println("start");
		return null;
	}

	@Override
	public void stop() {
	}

}
