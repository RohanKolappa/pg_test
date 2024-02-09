package com.ipvs.sessionservice.resource.handler;

import java.util.ArrayList;
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
import com.ipvs.sessionservice.handler.base.GetBaseObjectListRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class GetTagsRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI {

	@Override
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

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document doc = request.getMessage().getDocument();
		Element getTagElement = MRXMLUtils.getElementXpath(doc, "//GetTags");
		String objectType = MRXMLUtils.getAttributeValue(getTagElement, "whereObjectType", "");
		if ("".equals(objectType)){
			objectType = SessionServiceConst.ALL;
		}
		
		if(!SessionServiceResourceConst.OBJECT_TYPE_ENUMS.contains(objectType))
			throw new MRException (SessionServiceExceptionCodes.OBJECTTYPE_DOES_NOTEXISTS,MRXMLUtils.generateErrorXML("objecttype", objectType));
		
		List<String> listOfXPath = new ArrayList<String>();
		String whereNameSubstring = MRXMLUtils.getAttributeValue(getTagElement, SessionServiceConst.WHERE_NAME_TEXT_SUBSTRING, "");
		String whereValueSubString = MRXMLUtils.getAttributeValue(getTagElement, SessionServiceConst.WHERE_VALUE_TEXT_SUBSTRING, "");
		if(!whereNameSubstring.isEmpty()) {
			listOfXPath.add(" matches(Groups/Tag/@Name, '" + whereNameSubstring + "', 'i')  ");
		}

		if(!whereValueSubString.isEmpty()){
			listOfXPath.add(" matches(Groups/Tag/@Value, '" + whereValueSubString + "', 'i')  ");
		}
		String sortBy = MRXMLUtils.getAttributeValue(getTagElement, SessionServiceConst.SORT_BY);

		if(sortBy.equals("Value")){
			sortBy = "@Value";
		} else {
			sortBy = "@Name";
		}
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.GETTAGS + SessionServiceConst.XQUERYXML;
		Document responseDoc = getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getTagElement, objectType, sortBy, listOfXPath, xqueryFileName);
		return responseDoc;
	}

}
