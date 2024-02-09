package com.ipvs.sessionservice.handler.base;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
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
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.window.handler.DisplayUtils;
import com.ipvs.sessionservice.window.handler.SessionServiceWindowConst;

public abstract class SetBaseServiceClientDisplayRequestHandler implements MRRequestHandlerI {
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

	protected Document updateDisplay(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String displayCommandRequestName,String displayCommandELName )  throws MRException,Exception{
		
		String displayXPath =  "//"+displayCommandELName+"/"+SessionServiceConst.DISPLAY;
		String canvasXPath = "//"+displayCommandELName+"/"+SessionServiceConst.CANVAS;
	
		Document requestDoc = request.getMessage().getDocument();

		Element displayEl = MRXMLUtils.getElementXpath(requestDoc,displayXPath );
		Element canvasEl = MRXMLUtils.getElementXpath(requestDoc, canvasXPath );
		Element setServiceClientRequestEl = MRXMLUtils.getElementXpath(requestDoc, "//"+displayCommandRequestName);
		String deviceNID = MRXMLUtils.getAttributeValueWithAlternate(setServiceClientRequestEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, SessionServiceConst.WHERE_NID, "");
		if(displayCommandRequestName.equals(SessionServiceConst.SET_SERVICE_CLIENT_DISPLAY_STATUS) || displayCommandRequestName.equals(SessionServiceConst.SET_SERVICE_CLIENT_DISPLAY_CONFIG)) {
			
			String notificationType = SessionServiceWindowConst.DISPLAY_CONFIG;
			if(displayCommandRequestName.equals(SessionServiceConst.SET_SERVICE_CLIENT_DISPLAY_STATUS)) {
				notificationType = SessionServiceWindowConst.DISPLAY_STATUS;
			}
			
			Element displayStatusEL = MRXMLUtils.newElement(notificationType);
			if(displayEl != null)
				MRXMLUtils.addElement(displayStatusEL,(Element)displayEl.clone());
			if(canvasEl != null)
				MRXMLUtils.addElement(displayStatusEL,(Element)canvasEl.clone());

			DisplayUtils.queueDisplayNotification(request, xmpp, log, db,serviceAgentFactory, null, deviceNID,displayStatusEL, SessionServiceConst.IPVSCLIENTDISPLAYCONFIG,"1",notificationType,true);
		}
		
		//update display element
		if(displayEl != null) {
			AssetAdminServiceUtils.updateChild(deviceNID, displayXPath, displayEl, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		}

		//update canvas element
		if(canvasEl != null) {
			AssetAdminServiceUtils.updateChild(deviceNID, canvasXPath, canvasEl, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		}
		Element responseEL = AssetAdminServiceUtils.getChild(deviceNID, "//"+displayCommandELName, request, log, db, xmpp, resourceFactory, serviceAgentFactory);

		MRXMLUtils.removeContent(setServiceClientRequestEl);
		MRXMLUtils.addElement(setServiceClientRequestEl,(Element) MRXMLUtils.getElementXpath(responseEL, "./"+displayCommandELName).clone());

		return request.createResponseData((Element)setServiceClientRequestEl.clone());

	}


}
