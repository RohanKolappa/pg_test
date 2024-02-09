package com.ipvs.mediaroomservice.streamingserver;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;

public class DummyStreamingServerImpl extends StreamingServerImpl {

	ServiceMessageHandlerI messageHandler = null;

    public DummyStreamingServerImpl(String userJID, MRLog log, MRDB db,XMPPF xf, ServiceMessageHandlerI messageHandler) {
        super(userJID, log, db,xf);
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
    	String sscmd = "<SS>" + userJID + ":" + cmd + "</SS>";
    	if(this.messageHandler != null)
    		this.messageHandler.handleServiceMessage(this.userJID, sscmd);
    	else
    		System.out.println(sscmd);
    	return "200 OK";
    }

    protected String setupEncoderPort(Element inputElement, Document streamDataDoc) {
        String srcPortElementID = MRXMLUtils.getAttributeValue(inputElement,"sourcePortElementID");         
    	this.log.addInfo("StreamingServerImpl::setupEncoderPort", MRLog.OK, MRLog.NOTIFY);
    	return srcPortElementID;
    }
    
    protected void teardownEncoderPort(Element inputElement) {
    	this.log.addInfo("StreamingServerImpl::teardownEncoderPort", MRLog.OK, MRLog.NOTIFY);
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

	@Override
	protected void simulateRecordProgressStoppedNotfication(Element outputElement,String ssInstanceNID) throws MRException,Exception {
		sendProgressStoppedSSEventToSourceStreamAgent(outputElement, ssInstanceNID);
	}
    

    // END Methods overridden in DeviceStreamingServerImpl

}
