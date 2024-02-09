package com.ipvs.mediaroomservice.impl;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.streamingserver.DummyStreamingServerImpl;

public class DummySourceStreamAgent extends SourceStreamAgent {
	
	ServiceMessageHandlerI messageHandler = null;
	
	public DummySourceStreamAgent(ServiceMessageHandlerI messageHandler) {
		this.messageHandler = messageHandler;
	}

	public void init(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
    	System.out.println("AppletSourceStreamAgent::init");
		DummyStreamingServerImpl dummyStreamingServer = new DummyStreamingServerImpl(userJID, log, db, xf, this.messageHandler);
        String deviceNID  = null;
        Document deviceXML = null;
        try {
        	deviceNID = MRXMLUtils.getValueXpath(setServiceDomainResponseDoc, "//deviceNID", "");
        	if(deviceNID!= null) {
    			Document getDeviceResponseDoc =  MediaRoomServiceUtils.getAssetAdminResponse(MRRequest.createRequest(userJID), xf.xmpp(userJID), log,  db,  resourceFactory,  xf.sf(userJID),  "assetadminservice",
    					"AssetResource_BASE_ALL_GET.xml",  deviceNID,  "0",  null,  "Get");
    			Element deviceEL = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(getDeviceResponseDoc, "//ResponseXML"));
    			deviceXML = MRXMLUtils.elementToDocument(deviceEL);
    			dummyStreamingServer.getNetworkResource().updateNetworkResource(deviceXML);
        	}	
		} catch (Exception e) {
			deviceNID = null;
			e.printStackTrace();
		}
    	this.initInternal(xf, log, db, resourceFactory, userJID, deviceNID, null, dummyStreamingServer);
	}
}
