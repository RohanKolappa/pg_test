package com.ipvs.sessionservice.resource.handler;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
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
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class GetDeviceRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI {
	
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
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
			Document doc = request.getMessage().getDocument();
			Element getDeviceReqEl = MRXMLUtils.getElementXpath(doc, "//" + MRRequest.getRequestRootName(request.getRequestName()));											

			String whereTitle = MRXMLUtils.getAttributeValue(getDeviceReqEl, SessionServiceConst.WHERE_TITLE, "");
			String whereType = MRXMLUtils.getAttributeValue(getDeviceReqEl, SessionServiceConst.WHERE_TYPE, "");
						
			List<String> listOfWhereXpath = new ArrayList<String>();			
			String whereTitleXPath = "";
			if (!"".equals(whereTitle)) {
				whereTitleXPath = GetObjectUtils.generateXPath(whereTitle, SessionServiceResourceConst.TITLE, SessionServiceConst.DEVICE, SessionServiceConst.TITLE_CONST);
				if(!whereTitleXPath.isEmpty())
					listOfWhereXpath.add(whereTitleXPath);
			}
				
			String whereTypeXPath = "";
			if (!"".equals(whereType)) {
				whereTypeXPath = GetObjectUtils.generateXPath(whereType, SessionServiceConst.WHERE_TYPE, SessionServiceConst.DEVICE, SessionServiceConst.TYPE_CONST);
				if(!whereTypeXPath.isEmpty())
					listOfWhereXpath.add(whereTypeXPath);
			}
				
			String sortBy = MRXMLUtils.getAttributeValue(getDeviceReqEl, SessionServiceConst.SORT_BY, AMConst.STATEFLAG);
			if (sortBy.equalsIgnoreCase(SessionServiceResourceConst.TITLE)) {
				sortBy =  SessionServiceResourceConst.TITLE;
			} else if (sortBy.equalsIgnoreCase(SessionServiceConst.TYPE_ELEMENT)) {
				sortBy =  SessionServiceConst.TYPE_ELEMENT;
			} else if (sortBy.equals(SessionServiceConst.STATE)) {
	        	sortBy =  AMConst.STATEFLAG;
			} else if (sortBy.equals(SessionServiceConst.DESCRIPTION_ATTR_NAME)) {
				sortBy = AMConst.INFO+"/"+AMConst.DESCRIPTION;
			}
			
			String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.DEVICE + SessionServiceConst.XQUERYXML;		
			Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getDeviceReqEl, AMConst.AssetResource, sortBy, listOfWhereXpath, xqueryFileName);
			
			GetObjectUtils.subscribeUser(getDeviceReqEl, resultDoc, SessionServiceConst.DEVICE, request, serviceAgentFactory);
			
			return resultDoc;						
		}



}
