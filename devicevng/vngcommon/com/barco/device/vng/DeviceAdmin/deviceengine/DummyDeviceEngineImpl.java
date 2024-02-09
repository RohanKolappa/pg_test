/**
 * 
 */
package com.barco.device.vng.DeviceAdmin.deviceengine;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.vng.common.DeviceDB;

/**
 * @author NVAT
 *
 */
public class DummyDeviceEngineImpl extends BaseDeviceAdminEngineImpl {
	
	
	public DummyDeviceEngineImpl(Logger log, DeviceDB deviceDB){
		super(log, deviceDB);	
	}
	
	/* (non-Javadoc)
	 * @see com.barco.device.handler.DeviceAdmin.impl.DeviceServerI#setDeviceConfig(org.dom4j.Document)
	 */
	@Override
	public void setDeviceConfig(Document deviceConfigDoc) throws Exception {
	log.info("DummyDeviceServerImpl:SetDeviceConfig");
	}

	/* (non-Javadoc)
	 * @see com.barco.device.handler.DeviceAdmin.impl.DeviceServerI#getDeviceStatus(org.dom4j.Element)
	 */
	@Override
	public void getDeviceStatus(Element tempDeviceStatusElement)
			throws Exception {
	
		log.info("DummyDeviceServerImpl:GetDeviceStatus");
	}

	/* (non-Javadoc)
	 * @see com.barco.device.handler.DeviceAdmin.impl.DeviceServerI#resetServer()
	 */
	@Override
	public void resetServer() {
	this.setResetServer(true);

	}

	@Override
	protected void handleNotification(String notification) {	
		
	}

	@Override
	public void reboot() throws Exception {
		System.out.println("Device Rebooted");
		
	}

	@Override
	public void upgrade(String url) throws Exception {
		System.out.println("Device Upgraded With URL:="+url);
	}

	@Override
	public void revertToDefault(boolean revertNetworkConfig, boolean deleteMedia)
			throws Exception {
		System.out.println("device reverted to default");		
	}

	@Override
	public void saveConfiguration() throws Exception {
		System.out.println("Device Configuration Saved");
	}

	@Override
	public void shutdown() throws Exception {
		System.out.println("Device is Shutdown");
	}

	@Override
	public void haTakeover() throws Exception {
		System.out.println("HA Takeover complete");
		
	}

}
