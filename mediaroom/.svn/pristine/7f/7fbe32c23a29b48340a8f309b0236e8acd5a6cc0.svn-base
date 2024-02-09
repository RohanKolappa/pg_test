package com.ipvs.common.service;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;

public class DummyDeviceAdminServerAgent implements ServiceAgentI {


	MRLog log=null;
	
	public DummyDeviceAdminServerAgent() {
	}

	@Override
	public boolean handleEvent(MRClient client, Document doc, Element event)
	throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public String handleMessage(MRClient client, String fromJID, String message) throws Exception {
		
		Document doc = MRXMLUtils.stringToDocument(message);
		String clientCallback = MRXMLUtils.getValueXpath(doc, "//"+MRConst.CLIENT_CALLBACK, "");
		String  requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, null);
		Element deviceAdminResponse=null;
		String responseName=null;
		if (MRServiceConst.DEVICESTATUSREQUEST.compareToIgnoreCase(requestName)== 0) {
	        	responseName = MRServiceConst.DEVICESTATUSRESPONSE;
		} else if (MRServiceConst.DEVICECOMMANDREQUEST.compareToIgnoreCase(requestName)== 0) {
        	responseName = MRServiceConst.DEVICECOMMANDRESPONSE;
		} else if (MRServiceConst.DEVICECONFIGURATIONREQUEST.compareToIgnoreCase(requestName)== 0) {
        	responseName = MRServiceConst.DEVICECONFIGURATIONRESPONSE;
		}
            	      
		client.getLog().addInfo("DeviceAdminServerAgent: Received Request from DeviceAdminClientAgent=" + fromJID +" RequestName:="+requestName,MRLog.INFO,MRLog.NOTIFY);

		deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");
		
		int requestState = MRRequest.STATE_DONE;
		String logdata = "";		
		 
		ServiceAgentUtils.sendAgentResponseMessage(client.getXmpp(),client.getLog(), 
				null, fromJID, clientCallback, responseName, MRXMLUtils.elementToString(deviceAdminResponse), 
				"DeviceAdminServerAgent: Sending DeviceAdminResponse from " +
				MRServiceConst.DEVICEADMINSERVERAGENT + " to " + clientCallback,
				requestState ,
				logdata, doc);
		return message;

	}

	@Override
	public void init(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		// TODO Auto-generated method stub
		this.log=log;
		ServiceAgentUtils.sendAgentStateEvent(userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), DeviceAdminServerAgent.class.getName(), MediaStoreServiceConst.READY, xf.xmpp(userJID), this.log);
	}
	@Override
	public void reset() throws Exception {
		// TODO Auto-generated method stub

	}
}
