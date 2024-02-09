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

public class GetReportRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
	
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
				Element getReportElement = MRXMLUtils.getElementXpath(doc, "//" + SessionServiceConst.GET_REPORT_EL_NAME);											
				
				String whereName = MRXMLUtils.getAttributeValue(getReportElement, SessionServiceConst.WHERE_NAME_ATTRIBUTE, "");
				String whereType = MRXMLUtils.getAttributeValue(getReportElement, SessionServiceConst.WHERE_TYPE, "");
			
				List<String> listOfWhereXpath = new ArrayList<String>();			
				String whereNameXPath = "";
				if (!"".equals(whereName)) {
					whereNameXPath = GetObjectUtils.generateXPath(whereName, SessionServiceConst.NAME_EL_NAME, SessionServiceConst.REPORT_OBJECT_TYPE, SessionServiceConst.NAME_CONST);
					if(!whereNameXPath.isEmpty())
						listOfWhereXpath.add(whereNameXPath);
				}
				
				String whereTypeXPath = "";
				if (!"".equals(whereType)) {
					whereTypeXPath = GetObjectUtils.generateXPath(whereType, SessionServiceConst.WHERE_TYPE, SessionServiceConst.REPORT_OBJECT_TYPE, SessionServiceConst.TYPE_CONST);
					if(!whereTypeXPath.isEmpty())
						listOfWhereXpath.add(whereTypeXPath);
				}
				
				String sortBy = MRXMLUtils.getAttributeValue(getReportElement, SessionServiceConst.SORT_BY, "");
				if (sortBy.equals(SessionServiceConst.NAME)) {
					sortBy =  SessionServiceConst.NAME_EL_NAME;
				} else if (sortBy.equals(SessionServiceConst.TYPE_ATTRIBUTE)) {
					sortBy =  SessionServiceConst.TYPE_EL_NAME;
				} 
				
				String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.REPORT_OBJECT_TYPE + SessionServiceConst.XQUERYXML;		
				Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getReportElement, SessionServiceConst.REPORT_OBJECT_TYPE, sortBy, listOfWhereXpath, xqueryFileName);
						
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