package com.barco.device.vng.Media.streamingserver;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;

public class DummyStreamingServerImpl extends StreamingServerImpl {

	ServiceMessageHandlerI messageHandler = null;

    public DummyStreamingServerImpl(Logger log, DeviceDBI deviceDB, ServiceMessageHandlerI messageHandler) {
        super(log, deviceDB);
        this.messageHandler = messageHandler;
    }

    public boolean isExternal() {
    	return true;
    }
    
    // BEGIN Methods overridden in DeviceStreamingServerImpl
    public String getIPAddress() throws Exception {
    	return "192.168.1.1";
    }

    protected String executeSSCommand(String cmd) throws Exception {
    	String sscmd = "<SS>" + this.deviceDB.getDeviceResourceNID() + ":" + cmd + "</SS>";
    	if(this.messageHandler != null)
    		this.messageHandler.handleServiceMessage(this.deviceDB.getDeviceResourceNID(), sscmd);
    	else
    		System.out.println(sscmd);
    	return "200 OK";
    }

    protected String setupEncoderPort(Element inputElement, Document streamDataDoc) {
        String srcPortElementID = XMLUtils.getAttributeValue(inputElement,"sourcePortElementID");         
    	this.log.info("StreamingServerImpl::setupEncoderPort");
    	return srcPortElementID;
    }
    
    protected void teardownEncoderPort(Element inputElement) {
    	this.log.info("StreamingServerImpl::teardownEncoderPort");
    }

	@Override
	protected void setupDecoderPort(Element outputElement) {
		// TODO Auto-generated method stub
		
	}

	@Override
	protected void teardownDecoderPort(Element outputElement) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setupInternalInstances(Document doc) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public int getMaxEncoderInstances(String streamType) {
		// TODO Auto-generated method stub
		return 0;
	}

    protected String executeStatus(String statusCmd) throws Exception {
    	return "t:" + String.valueOf(System.currentTimeMillis());
    }


    // END Methods overridden in DeviceStreamingServerImpl

}
