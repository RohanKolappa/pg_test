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
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class GetUserRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
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
		
			try
			{
				Document doc = request.getMessage().getDocument();				
				Element getUserElement = MRXMLUtils.getElementXpath(doc, "//GetUser");											
				String whereUsername = MRXMLUtils.getAttributeValue(getUserElement, "whereUsername", "");
			
				List<String> listOfWhereXpath = new ArrayList<String>();			
				String whereUsernameXPath = "";
				if (!"".equals(whereUsername)) {
					whereUsernameXPath = GetObjectUtils.generateXPath(whereUsername, SessionServiceConst.USERNAME, SessionServiceConst.USER, SessionServiceConst.USER_NAME_CONST);
					if(!whereUsernameXPath.isEmpty())
						listOfWhereXpath.add(whereUsernameXPath);
				}
				
				String sortBy = MRXMLUtils.getAttributeValue(getUserElement, "sortBy", "");
				if (sortBy.equals(SessionServiceConst.USER_NAME_ATTRIBUTE)) {
					sortBy =  SessionServiceConst.USERNAME;
				} else if (sortBy.equals(SessionServiceConst.FULL_NAME)) {
					sortBy =  SessionServiceConst.FULLNAME;
				}
				
				String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.USER + SessionServiceConst.XQUERYXML;		
				Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getUserElement, SessionServiceConst.USER, sortBy, listOfWhereXpath, xqueryFileName);
				
				GetObjectUtils.subscribeUser(getUserElement, resultDoc, SessionServiceConst.USER, request, serviceAgentFactory);
				
				return resultDoc;
			}
			catch (MRException me)
			{
				System.out.println(me.getCode());
				System.out.println(me.getData());
				throw me;
			}
			catch(Exception e) 
			{
				e.printStackTrace();
				throw new MRException(SessionServiceExceptionCodes.OTHER, MRException.getStackTrace(e));
			}
		}
	}