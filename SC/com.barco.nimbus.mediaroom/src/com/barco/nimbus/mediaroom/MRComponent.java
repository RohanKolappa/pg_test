package com.barco.nimbus.mediaroom;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.osgi.framework.BundleContext;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.api.DBI;
import com.barco.nimbus.api.DBListenerI;
import com.barco.nimbus.api.MRComponentI;
import com.barco.nimbus.api.MessageBusI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.license.api.License;
import com.barco.nimbus.license.api.LicenseServiceCallbackI;
import com.barco.nimbus.license.api.LicenseServiceI;
import com.barco.nimbus.utils.common.XMLUtils;
import com.ipvs.nimbus.MRApp;
import com.ipvs.nimbus.MRSCException;
import com.ipvs.nimbus.MRSCExceptionCodes;
import com.ipvs.nimbus.MRSCMessage;

/*
 * The purpose of this class is to relay the messageBus and db calls coming from MRApp to the SC modules
 */

@Component
public class MRComponent implements MRComponentI, DBListenerI,
		LicenseServiceCallbackI {

	private MRComponentDB mrdb;
	private MRComponentMessageBus mrMessageBus;
	private MRApp mrApp;
	private DBI db;
	private MessageBusI messageBus;
	private LicenseServiceI licenseService;

	@Reference
	public void setMessageBus(MessageBusI mb) {
		Logger.getLogger(MRComponent.class).info("MRComponent:setMessageBus");
		this.messageBus = mb;
	}

	public void unsetMessageBus(MessageBusI mb) {
		Logger.getLogger(MRComponent.class).info("MRComponent:unsetMessageBus");
		this.messageBus = null;
	}

	@Reference
	public void setDb(DBI db) {
		Logger.getLogger(MRComponent.class).info("MRComponent:setDB");
		this.db = db;
		Logger.getLogger(MRComponent.class).info("MRComponent:setDB" + this.db);
	}

	public void unsetDb(DBI db) {
		Logger.getLogger(MRComponent.class).info("MRComponent:unsetDB");
		this.db = null;
	}

	@Reference
	public void setLicenseService(LicenseServiceI licenseService) {
		Logger.getLogger(MRComponent.class).info(
				"MRComponent:setLicenseService");
		this.licenseService = licenseService;
		licenseService.registerCallback(this);
	}

	public void unsetLicenseService(LicenseServiceI licenseService) {
		Logger.getLogger(MRComponent.class).info(
				"MRComponent:unsetLicenseService");
		this.licenseService = null;
	}

	@Activate
	public void start(BundleContext context) throws Exception {
		Logger.getLogger(MRComponent.class).info("MRComponent:start; starting...");
		
		Logger.getLogger(MRComponent.class).debug("MRComponent:start; registring db listener.");
		this.db.registerListener(this);
		
		Logger.getLogger(MRComponent.class).debug("MRComponent:start; handle connection started.");
		Logger.getLogger(MRComponent.class).info("MRComponent:handleConnected");
		
		mrdb = new MRComponentDB(this.db);
		mrMessageBus = new MRComponentMessageBus(this.messageBus);
		mrApp = new MRApp();
		
		Logger.getLogger(MRComponent.class).debug("MRComponent:start; connection handled.");
		
		Logger.getLogger(MRComponent.class).info("MRComponent:start; started.");
	}

	@Deactivate
	public void stop() throws Exception {
		Logger.getLogger(MRComponent.class).info("MRComponent:stop");
	}

	public void handleMessage(SCMessage message) throws SCException, Exception {
		try {
			MRSCMessage mrSCMessage = new MRSCMessage(
					message.getMessageBusClassName(), message.getMessageDoc(),
					message.getType(), message.getService(),
					message.getRootName(), message.getClientData(),
					message.getClientCallback(), message.getFrom(),
					message.getTo(), message.getLogLevel());

			// TODO: enforce license
			/*if (!licenseService.get().isLicenseValid()) {
				throw new SCException(MRSCExceptionCodes.LICENSE_EXPIRY_CODE,
						"License Expired, Please upload license.");
			}*/
			this.mrApp.handleMessage(mrSCMessage);
			String resultStr = mrSCMessage.getResultString();
			if (resultStr != null) {
				Document doc = XMLUtils.stringToDocument(resultStr);
				message.setResult(doc);
			}
		} catch (MRSCException me) {
			// me.printStackTrace();
			throw new SCException(me.getCode(), me.getData(),
					me.getOriginException(), me.getOriginData());
		}
	}

	@Override
	public void handleConnected() {
		//TODO unimplemented
	}

	@Override
	public void handleDisconnected() {
		Logger.getLogger(MRComponent.class).info(
				"MRComponent:handleDisconnected");
		// TODO stop required services for license expiry here.
	}

	/**
	 * @see com.barco.nimbus.license.api.LicenseServiceCallbackI#handleLicenseUpdated(com.barco.nimbus.license.api.License)
	 */
	@Override
	public void handleLicenseUpdated(License aLicense) {

		Logger.getLogger(MRComponent.class)
				.info("MRComponent:handleLicenseUpdated; Received notification from license service.",
						aLicense);

		switch (aLicense.getStatus()) {
		case LicenseAccepted: {
			Logger.getLogger(MRComponent.class)
					.info("MRComponent:handleLicenseUpdated; License accepted, connecting...");
			break;
		}
		case LicenseExpired: {
			Logger.getLogger(MRComponent.class)
					.info("MRComponent:handleLicenseUpdated; License expired, disconnecting...");
			handleDisconnected();
			// TODO: notify/handle all connected clients
			break;
		}
		case TrialLicense: {
			Logger.getLogger(MRComponent.class)
					.info("MRComponent:handleLicenseUpdated; Trial license accepted, connecting...");
			break;
		}
		default: {
			// intentionally blank
		}
		}
	}

	@Override
	public void startService() throws SCException, Exception {
		mrApp.start(mrMessageBus, mrdb);
	}
}
