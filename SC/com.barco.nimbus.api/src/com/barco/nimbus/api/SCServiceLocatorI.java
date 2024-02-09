package com.barco.nimbus.api;

import com.barco.nimbus.license.api.LicenseServiceI;

public interface SCServiceLocatorI {
	/**
	 * @return the message bus
	 */
	public MessageBusI getMessageBus();
	
	/**
	 * @return the db
	 */
	public DBI getDb();
	
	/**
	 * @return the mrComponent
	 */
	public MRComponentI getMRComponent();
	
	/**
	 * @return license service module
	 */
	public LicenseServiceI getLicenseService();
}
