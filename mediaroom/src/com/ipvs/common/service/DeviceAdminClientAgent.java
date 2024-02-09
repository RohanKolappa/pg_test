package com.ipvs.common.service;

import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;


public class DeviceAdminClientAgent implements ServiceAgentI {
    
	private Hashtable<String, XMPPTransaction> requestTable;
    
    public DeviceAdminClientAgent() {
        requestTable = new Hashtable<String, XMPPTransaction>();
    }

    @Override
    public boolean handleEvent(MRClient client, Document doc, Element event)
        throws Exception {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public String handleMessage(MRClient client, String fromJID, String message)
        throws Exception {
        // TODO Auto-generated method stub
        Document doc = MRXMLUtils.stringToDocument(message);
      
        client.getLog().addInfo("DeviceAdminClientAgent: Received Response from DeviceAdminServerAgent=" + fromJID);
        Element deviceStatusEL = MRXMLUtils.getElementXpath(doc, "//DeviceStatus");        
        String  requestNID = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NID,"");

        //Element user = MRXMLUtils.getElementXpath(doc, "//user");
        XMPPTransaction t = this.requestTable.get(requestNID);

        if (t != null) {
            t.setResponse(MRXMLUtils.elementToString(deviceStatusEL));
            client.getLog().addInfo("DeviceAdminClientAgent: Received Response from DeviceAdminServerAgent=" + fromJID);
        }       
        return message;
    }

    @Override
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID,Document setServiceDomainResponseDoc) throws Exception {
        // TODO Auto-generated method stub
		ServiceAgentUtils.sendAgentStateEvent(userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), DeviceAdminClientAgent.class.getName(), MediaStoreServiceConst.READY, xf.xmpp(userJID), log);
    }

    public String doDeviceAdminRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String deviceAdminAgentJID = null; //Derive this from the request
        deviceAdminAgentJID = AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(),serviceAgentFactory); 
        	//AppServerAgent.readAppServerInfoFromDB(xmpp, log, db, resourceFactory, serviceAgentFactory, request.getUserJID());

        return doDeviceAdminRequestWithAgentJID(deviceAdminAgentJID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }
    
    public String doDeviceAdminRequestWithAgentJID(String agentJID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    String deviceAdminAgentJID = null; //Derive this from the request
    deviceAdminAgentJID = agentJID;

    Element data = MediaRoomServiceUtils.cloneDataElement(request);
 
    XMPPTransaction t = new XMPPTransaction(request.getNID(), "doXMPPAdminRequest", deviceAdminAgentJID);
    this.requestTable.put(t.getId(), t);

    MRXMLUtils.addElement(data,MRXMLUtils.tagToElement("<requestNID>" + t.getId() + "</requestNID>"));

    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null,MRServiceConst.DEVICEADMINCLIENTAGENT, deviceAdminAgentJID, MRServiceConst.DEVICEADMINSERVERAGENT, 
    		MRServiceConst.DEVICESTATUSREQUEST, MRXMLUtils.elementToString(data),
        "DeviceAdminClientAgent: Sending DeviceAdminRequest from " + MRServiceConst.DEVICEADMINCLIENTAGENT + " to " + MRServiceConst.DEVICEADMINSERVERAGENT, request.getNID(),request.getUserJID(),MRConst.DEVICE_ADMIN_SERVICE,"");

    log.addInfo("DeviceAdminClientAgent: Waiting for DeviceAdminResponse from " + MRServiceConst.DEVICEADMINSERVERAGENT + " fromJID=" + deviceAdminAgentJID, MRLog.OK, MRLog.NOTIFY);

    t.waitForResponse();
    
    if(t.getResponse()==null){
    	System.out.println("Warning: Device Status response XML is null");
    	throw new MRException(MRExceptionCodes.GET_DEVICE_STATUS_FAILED,"Get Device Status Failed");
    }

    log.addInfo("DeviceAdminClientAgent: Done Waiting for DeviceAdminResponse from " + MRServiceConst.DEVICEADMINSERVERAGENT + " fromJID=" + deviceAdminAgentJID, MRLog.OK, MRLog.NOTIFY);

    this.requestTable.remove(t.getId());

    String response = (String) t.getResponse();

    if ("".equals(response.trim())) {
        System.out.println("xmppAdmin Exception response=" + response);
        throw new MRException(MRExceptionCodes.DEVICEADMIN_FAILED, response);
    }

    return (String) t.getResponse();
}

    public void sendGetDeviceStatusRequest(String agentJID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String deviceAdminAgentJID = null; //Derive this from the request
        deviceAdminAgentJID = agentJID;
        Element data = MediaRoomServiceUtils.cloneDataElement(request);
        String clientData = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//"+MRConst.CLIENT_DATA, "");
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, MRServiceConst.DEVICEADMINCLIENTAGENT, deviceAdminAgentJID, MRServiceConst.DEVICEADMINSERVERAGENT, MRServiceConst.DEVICESTATUSREQUEST, MRXMLUtils.elementToString(data),
                "DeviceAdminClientAgent: Sending DeviceAdminRequest from " + MRServiceConst.DEVICEADMINCLIENTAGENT + " to " + MRServiceConst.DEVICEADMINSERVERAGENT, request.getUserJID(), request.getNID(),MRConst.DEVICE_ADMIN_SERVICE, clientData);

    }

	@Override
	public void reset() throws Exception {
		// TODO Auto-generated method stub
		
	}
}
