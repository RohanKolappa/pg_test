package com.ipvs.sessionservice.session.handler;

import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
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
import com.ipvs.utils.Utils;

public class GetSessionRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI{

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
	
		Element sessionListElement = MRXMLUtils.getElementXpath(doc, "//GetSession");
		String countToFetchStr = MRXMLUtils.getAttributeValue(sessionListElement, "countToFetch", "-1");
		int countToFetch = Utils.getIntValue(countToFetchStr, -1);
		String selectFormat = MRXMLUtils.getAttributeValue(sessionListElement, "selectFormat", "BASIC");
		//String sortBy = MRXMLUtils.getAttributeValue(sessionListElement, "sortBy", "/Info/Title");
		//String sortOrder = MRXMLUtils.getAttributeValue(sessionListElement, "sortOrder", "descending");
		String startFromStr = MRXMLUtils.getAttributeValue(sessionListElement, "startFrom", "1");
		int startFrom = Utils.getIntValue(startFromStr,1);
		//String whereDateRange = MRXMLUtils.getAttributeValue(sessionListElement, "whereDateRange", "");
		//String whereNID = MRXMLUtils.getAttributeValue(sessionListElement, "whereNID", "");
		//String whereTagExpr = MRXMLUtils.getAttributeValue(sessionListElement, "whereTagExpr", "");
		//String whereTextSubStr = MRXMLUtils.getAttributeValue(sessionListElement, "whereTextSubStr", "");
		String whereId = MRXMLUtils.getAttributeValue(sessionListElement, SessionServiceConst.WHERE_ID, "");
		String whereType = MRXMLUtils.getAttributeValue(sessionListElement, SessionServiceConst.WHERE_TYPE, "");	
				
		if (startFrom == 0){
			throw new MRException(SessionServiceExceptionCodes.INVALID_STARTFROM, "Invalid StartFrom");
		}
		
		List<?> sessionList = null;
		String whereXpath = "";
		StringBuffer buf = new StringBuffer();

		if (!"".equals(whereId) || (!"".equals(whereType))) {
			if (!"".equals(whereId)) {
				buf = this.generateXpath(buf, whereId, "id");
				log.addInfo("whereId=" + whereId, MRLog.OK, MRLog.NOTIFY);
			}
			
			if (!"".equals(whereType)) {
				if (buf.length() > 0) {
					buf.append(" and ");
				}
				// buf.append("@type = '" + whereType + "'");
				buf = this.generateXpath(buf, whereType, "type");
			}

			if (buf.length() > 0) {
				whereXpath= "//"+SessionServiceConst.SESSION + "["+buf.toString()+"]";
			}
		}
		log.addInfo("whereXpath=" + whereXpath, MRLog.OK, MRLog.NOTIFY);

		
		Document sessionListDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionList(request, xmpp, log, db,resourceFactory, serviceAgentFactory, startFrom,countToFetch, whereXpath);
		sessionList = MRXMLUtils.getListXpath(sessionListDoc, "//"+ SessionServiceConst.SESSION);

			
		sessionListDoc = MRXMLUtils.stringToDocument("<GetSession from='"+ startFrom +"' total='"+MRXMLUtils.getAttributeValue(sessionListDoc.getRootElement(),"totalRecords")+"'/>" );        			

		if(sessionList != null && selectFormat.equals("BASIC")) {
			for (int i=0; i <sessionList.size(); i++){
				Document sessionDoc = MRXMLUtils.elementToDocument((Element) sessionList.get(i));
				String id = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.SESSION,"id", "");
				String name = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.SESSION,"name", "");
				String state = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.SESSION,"state", "");
				String type = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.SESSION,"type", "");
				String timelineId = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.SESSION,"timelineId", "");
				String recordTimelineId = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.SESSION,"recordTimelineId", "");
				Element sessionEl = MRXMLUtils.stringToElement("<Session id='"+id+"' name='"+name+"' state='"+state+"' type='"+type+
						"' timelineId='"+timelineId+"' recordTimelineId='"+recordTimelineId+ "'/>");
				sessionEl.add(MRXMLUtils.getElementXpath(sessionDoc,"//Properties").detach());
				MRXMLUtils.addElement(sessionListDoc.getRootElement(), sessionEl);
			}
		} else {
			if(sessionList!=null)
				MRXMLUtils.addChildrenToElement(MRXMLUtils.getElementXpath(sessionListDoc, "//GetSession"),sessionList);	
		}
		return request.createResponseData(sessionListDoc.getRootElement());
	}
	
	public StringBuffer generateXpath(StringBuffer buf,String wherXpath, String attrType){
	    StringTokenizer st = new StringTokenizer(wherXpath, ",");
	    buf.append("@"+attrType+"[ .= '");
	    String delimiter = "' or .= '";
	        while (st.hasMoreTokens()) {
	            String token = st.nextToken();
	            token = token.trim();
	            if(st.hasMoreTokens())
	        	buf.append(token+delimiter);
	            else
	        	buf.append(token+"']");
	        }
	    return buf;
	}
}
