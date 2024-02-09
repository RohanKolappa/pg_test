package com.ipvs.common.service;

import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;

public abstract class ServiceClientAgent implements ServiceAgentI {

	protected XMPPI xmpp;
	protected MRLog log;
	protected MRResourceFactoryI resourceFactory;
	protected ServiceAgentFactoryI profileFactory;
	protected String userJID;

	private Hashtable<String, XMPPTransaction> requestTable;

	public ServiceClientAgent() {
		this.requestTable = new Hashtable<String, XMPPTransaction>();
	}

	public void init(XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory, String userJID,String deviceNID) {
		this.xmpp = xmpp;
		this.log = log;
		this.resourceFactory = resourceFactory;
		this.profileFactory = profileFactory;
		this.userJID = userJID;
	}

	public boolean handleEvent(MRClient client, Document doc, Element event) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	protected void setRequest(String requestNID, String client, String clientdata) {
		XMPPTransaction t = new XMPPTransaction(requestNID, client, clientdata);
		this.requestTable.put(requestNID, t);
	}

	protected void setResponse(String requestNID, Object response) throws Exception {
		XMPPTransaction t = this.requestTable.get(requestNID);
		t.setResponse(response); 
	}

	protected Object readResponse(String requestNID) throws Exception {
		return this.readResponse(requestNID, XMPPTransaction.WAIT_TIME);
	}

	protected Object readResponse(String requestNID, long timeout) throws Exception {
	    XMPPTransaction t = this.requestTable.get(requestNID);
	    log.addInfo("ServiceClientAgent: Waiting for Response cliendata=" + t.clientdata);
        t.waitForResponse(timeout);
	    log.addInfo("ServiceClientAgent: Received Response cliendata=" + t.clientdata);
	    this.requestTable.remove(requestNID);    
	    return t.getResponse();
	}

	public String handleMessage(MRClient client, String fromJID, String message) throws Exception {
		Document doc = MRXMLUtils.stringToDocument(message);
		String requestNID = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH,MRConst.REQUEST_NID, "");
		if("".equals(requestNID))
			return message;
		this.setResponse(requestNID, doc);
		return message;
	}

	public Document getResponse(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
			String serverAgentJID, String agentName) throws Exception {
		this.setRequest(request.getNID(), this.getClass().getName(), serverAgentJID);
		ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, 
				ServiceClientAgent.class.getName(), null, 
				serverAgentJID, agentName, request.getMessage().document, 
				"ServiceClientAgent: Sending serviceRequest from " + this.getClass().getName() +
				" to " + agentName,  request.getNID());
		return (Document)this.readResponse(request.getNID());
	}

}
