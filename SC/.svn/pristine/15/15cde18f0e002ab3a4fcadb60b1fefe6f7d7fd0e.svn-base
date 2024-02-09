package com.barco.nimbus.scservice;

import org.apache.log4j.Logger;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.api.MRComponentI;
import com.barco.nimbus.api.MessageBusI;
import com.barco.nimbus.api.SCServiceLocatorI;
import com.barco.nimbus.license.api.LicenseServiceI;

@Component
public class SCServiceLocator implements SCServiceLocatorI {

	private MessageBusI messageBus;
	private MRComponentI mrComponent;
	private LicenseServiceI licenseService;
	private DBI db;

	private static Logger logger = Logger.getLogger(SCServiceLocator.class);

	@Reference
	public void setMRComponent(MRComponentI mrComponent) {
		logger.info(":setMRComponent");
		this.mrComponent = mrComponent;
		logger.info(":setMRComponent, " + this.mrComponent);
	}

	public void unsetMRComponent(MRComponentI db) {
		this.mrComponent = null;
		logger.info(":unsetMRComponent, " + this.mrComponent);
	}

	@Override
	public MRComponentI getMRComponent() {
		return this.mrComponent;
	}

	@Reference
	public void setMessageBus(MessageBusI mb) {
		logger.info(":setMessageBus");
		this.messageBus = mb;
		logger.info(":setMessageBus, " + this.messageBus);
	}

	public void unsetMessageBus(MessageBusI mb) {
		this.messageBus = null;
		logger.info(":unsetMessageBus, " + this.messageBus);
	}

	@Override
	public MessageBusI getMessageBus() {
		return this.messageBus;
	}

	@Reference
	public void setDb(DBI db) {
		logger.info(":setDb");
		this.db = db;
		logger.info(":setDb, " + this.db);
	}

	public void unsetDb(DBI db) {
		this.db = null;
		logger.info(":unsetDb, " + this.db);
	}

	public DBI getDb() {
		return this.db;
	}

	@Reference
	public void setLicenseService(LicenseServiceI licenseService) {
		logger.info(":setLicenseService");
		this.licenseService = licenseService;
		logger.info(":setLicenseService, " + this.licenseService);
	}

	public void unsetLicenseService(LicenseServiceI licenseService) {
		this.licenseService = null;
		logger.info(":unsetLicenseService, " + this.licenseService);
	}

	@Override
	public LicenseServiceI getLicenseService() {
		return this.licenseService;
	}

	@Activate
	public void start() throws Exception {
		logger.info(":start");
	}

	@Deactivate
	public void stop() throws Exception {
		logger.info(":stop");
	}

}
