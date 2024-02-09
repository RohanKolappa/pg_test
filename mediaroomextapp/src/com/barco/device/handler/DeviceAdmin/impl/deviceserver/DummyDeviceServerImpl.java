/**
 * 
 */
package com.barco.device.handler.DeviceAdmin.impl.deviceserver;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.utils.mr.MRLog;

/**
 * @author NVAT
 *
 */
public class DummyDeviceServerImpl extends BaseDeviceServerImpl {
	
	
	public DummyDeviceServerImpl(MRLog log, String configFile){
		super(log, configFile);	
	}
	
	/* (non-Javadoc)
	 * @see com.barco.device.handler.DeviceAdmin.impl.DeviceServerI#setDeviceConfig(org.dom4j.Document)
	 */
	@Override
	public void setDeviceConfig(Document deviceConfigDoc) throws Exception {
	log.addInfo("DummyDeviceServerImpl:SetDeviceConfig",MRLog.INFO,MRLog.NOTIFY);
	}

	/* (non-Javadoc)
	 * @see com.barco.device.handler.DeviceAdmin.impl.DeviceServerI#getDeviceStatus(org.dom4j.Element)
	 */
	@Override
	public void getDeviceStatus(Element tempDeviceStatusElement)
			throws Exception {
	
		log.addInfo("DummyDeviceServerImpl:GetDeviceStatus",MRLog.INFO,MRLog.NOTIFY);
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

}
