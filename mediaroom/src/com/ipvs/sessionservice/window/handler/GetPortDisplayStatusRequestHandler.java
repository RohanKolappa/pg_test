package com.ipvs.sessionservice.window.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.features.FeatureBase;
import com.ipvs.sessionservice.handler.base.PortBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class GetPortDisplayStatusRequestHandler extends PortBaseRequestHandler  implements MRRequestHandlerI  {
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

	
	
	public static Document translate(Document portDoc, Document xp200DisplayStatusDoc ) throws Exception {
		MRXMLUtils.removeContent(portDoc.getRootElement());
		portDoc.getRootElement().add(xp200DisplayStatusDoc.getRootElement().detach());
		return portDoc;
	}
	
	public Document translateDeviceToWindowAPI(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document portDoc) throws Exception {

		Document getPortDisplayStatusDocument = MRXMLUtils.stringToDocument("<GetPortDisplayStatus></GetPortDisplayStatus>");
		String xmlFilePath = "/com/ipvs/xsd/objectXml/PortDisplayConfigTemplate.xml";
		Document portDisplayStatusTemplate = MRXMLUtils.loadXMLResource(xmlFilePath, log);
		portDisplayStatusTemplate.getRootElement().setName("PortDisplayStatus");
		List<?>portList = MRXMLUtils.getChildren(portDoc.getRootElement(), SessionServiceConst.PORT);
		for(int i=0;portList!= null && i<portList.size();i++) {
			Element portElement = (Element)portList.get(i);
			String portNID = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.NID, "");
			String portTitle = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.TITLE, "");
			Document portDisplayStatusDoc = (Document)portDisplayStatusTemplate.clone();
			MRXMLUtils.setAttributeValueXPath(portDisplayStatusDoc, "//PortDisplayStatus", SessionServiceConst.NID , portNID);
			MRXMLUtils.setAttributeValueXPath(portDisplayStatusDoc, "//PortDisplayStatus", SessionServiceConst.TITLE, portTitle);
			FeatureBase featureBase = getSupportedFeature(portElement);
			if(featureBase == null) {
				throw new MRException(SessionServiceExceptionCodes.FEATURE_NOT_SUPPORTED, "Port doesn't support this feature");
			} else {
				portDisplayStatusDoc = featureBase.generateWindowAPIFromDeviceStatus(request,xmpp,log,db, resourceFactory,serviceAgentFactory, portDisplayStatusDoc, portElement);
			}

			getPortDisplayStatusDocument.getRootElement().add(portDisplayStatusDoc.getRootElement().detach());
		}
		return getPortDisplayStatusDocument;
	}	
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
		Document portDoc = getPortDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, requestObject);
		GetObjectUtils.subscribeUser(requestObject.getRequestElement(), portDoc, SessionServiceConst.PORT, request, serviceAgentFactory,SessionServiceWindowConst.DISPLAY_STATUS);
		Document translatedDoc = translateDeviceToWindowAPI(request, xmpp, log,
				db, resourceFactory, serviceAgentFactory, portDoc);
		Document responseDoc = MRXMLUtils.stringToDocument("<GetPortDisplayStatusResponseData />");
		responseDoc.getRootElement().add(translatedDoc.getRootElement().detach());
        return responseDoc;
	}
	
}
