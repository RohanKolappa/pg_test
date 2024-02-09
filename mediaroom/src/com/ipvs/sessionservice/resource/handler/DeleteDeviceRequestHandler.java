package com.ipvs.sessionservice.resource.handler;


import java.util.Arrays;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.DeleteObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class DeleteDeviceRequestHandler extends BaseDeviceRequestHandler {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
		
		log.addInfo("DeleteDeviceRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.AssetResource);
		
		Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, SessionServiceConst.SELECTFORMAT_EXTENDED, 
        		GET_DEVICE_REQUEST_XML_PATH, GetDeviceRequestHandler.class.getName());
		Element deviceEl = MRXMLUtils.getElementXpath(responseDoc, "//" + SessionServiceConst.DEVICE);
		if(deviceEl == null)
			throw new MRException(SessionServiceExceptionCodes.DEVICE_NOT_FOUND, "Device not found");
			
		String deviceState = MRXMLUtils.getAttributeValue(deviceEl, SessionServiceConst.STATE, "");
		if(deviceState.equals(SessionServiceConst.READY) || deviceState.equals(SessionServiceConst.NOT_READY))
			throw new MRException(MRExceptionCodes.CANNOT_DELETE_ONLINE_DEVICE, "Cannot Delete. Device is online.");
		String deviceNID  = MRXMLUtils.getAttributeValueXpath(deviceEl, ".//"+SessionServiceResourceConst.DEVICE_CONFIG_ELEMENT, SessionServiceConst.NID,"");
		String assetResourceNID = MRXMLUtils.getAttributeValue(deviceEl, SessionServiceConst.NID);
		List<String> PortNIDList = MRXMLUtils.getValuesXpath(deviceEl, ".//"+SessionServiceConst.PORT+"/@"+SessionServiceConst.NID);
		
		List<?> srcPortElementList = MRXMLUtils.getListXpath(responseDoc, "//Port[@portType='"+SessionServiceConst.STREAM_SRC+"' ]");
		
		for (Object object : srcPortElementList) {
			Element portElement = (Element) object;
			String streamType = MRXMLUtils.getAttributeValueXpath(portElement, "@streamType", "");
			Document portDoc = MRXMLUtils.elementToDocument(portElement);
			try {
				AppServerAgentUtils.getPVRManager(serviceAgentFactory).stopPersistentPVRSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portDoc);
			} catch (Exception e) {
				// Intentionally blank
			}
			if (MediaRoomServiceConst.RTP_RAW.equals(streamType) || MediaRoomServiceConst.RFB.equals(streamType) || MediaRoomServiceConst.RTP_PCM.equals(streamType) 
					|| MediaRoomServiceConst.V2D.equals(streamType)) {
				try {
					AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).stopPersistentAutoStartSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory,
							portDoc);
				} catch (Exception e) {
					// Intentionally blank
				}
			}
		}
		
			
		//get device doc from database
		Element deviceElFromDb = null;
		if(!deviceNID.isEmpty())
			deviceElFromDb = AssetAdminServiceUtils.getChild(deviceNID, "//"+SessionServiceResourceConst.RESOURCE_POINTERS_ELEMENT, request, log, db, xmpp, resourceFactory, serviceAgentFactory);   
		
		//delete ports
		for(String portNID:PortNIDList) {
			String portType = "";
			if(deviceElFromDb != null) {
				Element portObjectElement = MRXMLUtils.getElementXpath(deviceElFromDb, ".//"+SessionServiceResourceConst.NID_VALUE+"[.='"+portNID+"']");
				if(portObjectElement != null) {
					String portObjectElementName = MRXMLUtils.getElementXpath(deviceElFromDb, ".//"+SessionServiceResourceConst.NID_VALUE+"[.='"+portNID+"']").getParent().getName();
					portType = AssetAdminServiceUtils.getPortResourceTypeFromDevice(portObjectElementName);
				}   
			}
			
			if(portType.isEmpty() && MRXMLUtils.getAttributeValue(deviceEl, SessionServiceConst.TYPE_ATTRIBUTE, "").equals(AMConst.WEB_CLIENT_DEVICE)) {
				portType = SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE;
			}
			
			DeleteObjectUtils.deleteObjectByNID(request, xmpp, log, db, portType, portNID);
			
			Document portDocbyNID = MRXMLUtils.getDocumentXpath(responseDoc, "//" + SessionServiceConst.PORT + "[@" + SessionServiceConst.NID 
					+ "='" + portNID + "']");
			((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourceManager().queueObjectNotification(xmpp, request, log, db, SessionServiceConst.PORT, SessionServiceConst.PORT, portNID, SessionServiceConst.DELETE, portDocbyNID);
		}
		
		//delete Device Doc
		if(!deviceNID.isEmpty()) {
			DeleteObjectUtils.deleteObjectByNID(request, xmpp, log, db, AMConst.Device, deviceNID);
		}


		//delete asset resource
		DeleteObjectUtils.deleteObjectByNID(request, xmpp, log, db, AMConst.AssetResource, assetResourceNID);
		
		StreamPolicyUtils.handlePolicyChanged();
		String title = MRXMLUtils.getAttributeValue(deviceEl, SessionServiceConst.TITLE);
		log.addInfo("Deleted Device. Title:="+title,MRLog.INFO,MRLog.NOTIFY);

		return responseDoc;

	}
}