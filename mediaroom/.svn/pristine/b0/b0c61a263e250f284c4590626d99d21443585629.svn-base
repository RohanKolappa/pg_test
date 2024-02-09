package com.ipvs.sessionservice.session.handler;

import java.net.URLEncoder;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class SetPortIOTypeRequestHandler implements MRRequestHandlerI {
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		 log.addTraceBegin(request);

	        int authType = MediaRoomObject.ROOM_TYPEID;
	        int role = MediaRoomPEM.VISITOR_ROLE;
	        String token = "testCommonToken";
	        String authority = "testAuthority";
	        String roomPolicyNID = MRConst.NULL;
	        String streamPolicyNID = MRConst.NULL;
	        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
	        log.addTraceEnd(request);

	        return auth;
	}

	private Element sendDeviceConfigRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String deviceAgentJID, Element commandElement) throws MRException, Exception {
		Document deviceConfigurationRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/systemservice/xml/DeviceConfigurationRequest.xml", log);
		Element deviceConfig = MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, "//DeviceConfig");
		Element deviceConfigRequestEl = deviceConfig.getParent();
		MRXMLUtils.removeContent(deviceConfigRequestEl);
		deviceConfig = (Element)commandElement.clone();
		deviceConfig.setName("DeviceConfig");
		MRXMLUtils.addElement(deviceConfigRequestEl,(Element)commandElement.clone());

		String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
		String toAgentName = MRServiceConst.DEVICEADMINSERVERAGENT;
	
		String requestNID = MRServiceConst.DEVICECONFIGURATIONREQUEST + "_" + request.getUserJID();
		
		MRRequest deviceConfigurationRequest = MRRequest.createRequest(request.getUserJID(), MRLog.INFO,
			requestNID, "systemservice", MRServiceConst.DEVICECONFIGURATIONREQUEST,
			new MRMessage(deviceConfigurationRequestDoc), null);
		
		Element dataEL = MediaRoomServiceUtils.cloneDataElement(deviceConfigurationRequest);
		
		String dataStr = MRXMLUtils.elementToString(dataEL);
		String serviceName = deviceConfigurationRequest.getServiceName();
	    String requestName = deviceConfigurationRequest.getRequestName();
	    String info = request.getRequestName() + "::sendAgentRequestForUserRequest " + 
    	fromAgentName + " to " + toAgentName;
	    
		Document userRequestDoc = request.marshalRequestObject(log, this.getClass().getName());
		String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
	
    	ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, deviceAgentJID, toAgentName, requestName, dataStr,
	        		info, request.getNID(), request.getUserJID(), serviceName, clientData);
    	
    	return dataEL;
	}

	public Element setPortIOType(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Element portElement, String portNID, String portIOType)throws MRException, Exception {
		// Get agentJID
		Document portDoc = MRXMLDBUtils.readElement(request, log, db, portNID);
		String assetResourceNID = MRXMLUtils.getValueXpath(portDoc, ".//AssetResourceNID", "");
		
		// Get commandElement
		String whereXPath = "AssetResourceNID[.='"+assetResourceNID+"']";
    	String selectXML = "<Device>" +"{root($VAR)/*/@NID}{$VAR//AVCodecElementsConfig}" + "</Device>";
    	Element responseEL = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.Device,selectXML, whereXPath, request, db, log, xmpp);    	
    	//System.out.println("SetPortIOTypeRequestHandler:getCommandElement deviceElement=" + MRXMLUtils.elementToString(responseEL));
    	Element commandElement = MRXMLUtils.getElementXpath(responseEL, "./Device");
    	if(commandElement == null)
 			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, 
					"Resource Device not found portNID=" + portNID);
		MRXMLUtils.setValueXpath(commandElement, ".//PortIOType", portIOType);
		Element xp200PortConfigElement = MRXMLUtils.getElementXpath(commandElement, ".//XP200PortConfig");
		xp200PortConfigElement.addAttribute("Dirty", "MODIFY");
		Element infoElement = MRXMLUtils.getElementXpath(commandElement, ".//Info");
		infoElement.addAttribute("Dirty", "MODIFY");
		commandElement.setName("DeviceConfig");
		
		// TBD: trim the commandElement of unwanted items
		/*<DeviceConfig>
			<AVCodecElementsConfig>
			<XP200PortConfig Dirty="MODIFY">
			<PortID>1</PortID>
			<Info Dirty="MODIFY">
			<StreamType>V2D</StreamType>
			<Configurable></Configurable>
			<PortIOType>StreamDst</PortIOType>
			<PortIOType></PortIOType>
			</Info>
			        </XP200PortConfig>
			    </AVCodecElementsConfig>
			</DeviceConfig>
		 */
		
		// sendDeviceAdminRequest
		String portAgentJID = MRXMLUtils.getValueXpath(portDoc, ".//Contact/OwnerUserJID", "");
		return this.sendDeviceConfigRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portAgentJID, commandElement);
	}

	private Element getPort(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle) throws Exception {
		String whereTextSubStr = portTitle;
		Document getPortRequestDoc = MRXMLUtils.stringToDocument("<GetPort whereTextSubStr='" + whereTextSubStr + "' selectFormat='ADVANCED' />");
		MRRequest getPortRequest = MRRequest.createRequest(request, getPortRequestDoc, 
				request.getRequestName(), request.getServiceName());
		GetPortRequestHandler handler = new GetPortRequestHandler();
		Document response = handler.getResponse(getPortRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		List<?> portList = MRXMLUtils.getListXpath(response, "//GetPort/Port");
		if(portList.size() == 0)
 			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, 
					"Resource not found portTitle=" + portTitle);
		if(portList.size() > 1)
 			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND,
					"Multiple Resources with portTitle=" + portTitle);
		return (Element)portList.get(0);
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document doc = request.getMessage().getDocument();
		Element setIOPortTypeElement = MRXMLUtils.getElementXpath(doc, "//SetPortIOType");
		String portTitle = setIOPortTypeElement.attributeValue("portTitle", null);
		String ioType = setIOPortTypeElement.attributeValue("ioType", null);
		
		// Get Port Doc
		Element portElement = this.getPort(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle);
		String portState = portElement.attributeValue("state");    	
		if(!portState.equals(MRClientUtils.ASSET_STATE_READY)){
				throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND,
					"Port Resource not Ready portTitle=" + portTitle);
	 	}
		String portNID = portElement.attributeValue("NID");
	    String portIOType = portElement.attributeValue("portType");
	    Element responseElement = MRXMLUtils.stringToElement("<SetPortIOType/>");
	    if(!ioType.equals(portIOType))
	    	responseElement = this.setPortIOType(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portElement, portNID, ioType);
	    
		return request.createResponseData(responseElement);

	}

}
