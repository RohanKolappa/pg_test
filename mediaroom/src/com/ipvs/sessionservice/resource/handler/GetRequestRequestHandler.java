package com.ipvs.sessionservice.resource.handler;

import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class GetRequestRequestHandler implements MRRequestHandlerI {

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
	/**  GetRequestRequestHandler handles the two objecttypes. 
	 *   If objecttype is Session , then it gets the List of sessions in the room and returns as response	 
	 * 
	 *   Exception Codes
	 *   public static final int OBJECTTYPE_DOES_NOTEXISTS =  4002;
	 *   public static final int GET_BOOKMARK_LIST_FAILED = 2254;
	 */
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document doc = request.getMessage().getDocument();
		
		// Check if an xpath has been passed in
		String requestXpath = "";
		//Element xpathElement = MRXMLUtils.getElementXpath(doc, "//GetRequest/Where");
		/*if(xpathElement != null) {
			requestXpath = xpathElement.getAttributeValue("xpath");
			if(requestXpath == null)
				requestXpath = "";
		}*/
		
		Element objectListElement = MRXMLUtils.getElementXpath(doc, "//ObjectList");
		String startFromStr = MRXMLUtils.getAttributeValue(objectListElement, "startFrom", "0");
		int startFrom = Utils.getIntValue(startFromStr,0);
		String countToFetchStr = MRXMLUtils.getAttributeValue(objectListElement, "countToFetch", "1000");
		int countToFetch = Utils.getIntValue(countToFetchStr, 1000);
		String sortBy = MRXMLUtils.getAttributeValue(objectListElement, "sortBy", "/Info/Title");
		String sortOrder = MRXMLUtils.getAttributeValue(objectListElement, "sortOrder", "descending");
		String detail = MRXMLUtils.getAttributeValue(objectListElement, "detail", "BASIC");
		String objectType = MRXMLUtils.getAttributeValue(objectListElement, "type", "");	
		
		Document resultDoc=  null;
		
		if(SessionServiceConst.SESSION.equals(objectType)){
			String whereXpath = "//MediaRoom";
			if (!"".equals(requestXpath)){ // Overide
				whereXpath = requestXpath;
			}
			resultDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory,1,countToFetch,"");
			resultDoc = convertMediaRoomResponseToObjectList(resultDoc, objectType, startFromStr);
		} else if(MediaRoomServiceConst.MEDIAROOM.equals(objectType)){
			String whereXpath = "//MediaRoom";
			if (!"".equals(requestXpath)){ // Overide
				whereXpath = requestXpath;
			}
			resultDoc = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, whereXpath, false);
			resultDoc = convertMediaRoomResponseToObjectList(resultDoc, objectType, startFromStr);
		} else if ((SessionServiceConst.FILE.equals(objectType)) || (SessionServiceConst.DIR.equals(objectType))) {
			String whereXpath = "";
			if(SessionServiceConst.FILE.equals(objectType)) {
				whereXpath = "((Info/MediaStoreFileInfo/Type='MediaGroup') or (Info/MediaStoreFileInfo/MediaGroupID=''))";
				if (!"".equals(requestXpath)){ // Overide
					whereXpath = requestXpath + "and" + whereXpath;
					
				}
			} 
			if (!"".equals(requestXpath) && SessionServiceConst.DIR.equals(objectType)){ // Overide
				whereXpath = requestXpath + "and" + whereXpath;
				
			}
			whereXpath = "[ " + whereXpath + "]";
			String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + objectType + SessionServiceConst.XQUERYXML;
			resultDoc = this.getQueryList(request, xmpp, log, db,serviceAgentFactory,resourceFactory, startFrom, countToFetch, 
					whereXpath, sortBy, sortOrder, detail, 
					xqueryFileName);
		} else {
			throw new MRException (SessionServiceExceptionCodes.OBJECTTYPE_DOES_NOTEXISTS,MRXMLUtils.generateErrorXML("objecttype", objectType));
		}
		
		Element getResponseEL = MRXMLUtils.addElement(AMConst.GET_RESPONSE);
		MRXMLUtils.addAttribute(resultDoc.getRootElement(), "type", objectType);
		MRXMLUtils.addElement(getResponseEL,(Element)resultDoc.getRootElement().detach());
		return request.createResponseData(getResponseEL);		

		
	}
	
	private Document convertMediaRoomResponseToObjectList(Document resultDoc,String objectType,String startFrom)  throws MRException,Exception{
		
        List<?> objectList = null;
        int totalRecords=  0;
		
		if(resultDoc != null) {
			objectList = MRXMLUtils.getListXpath(resultDoc, "//"+objectType);
			if (objectList != null ) {
				totalRecords = objectList.size();
			}			 
		}
		
		Document ObjectListDoc = MRXMLUtils.stringToDocument("<ObjectList type='"+objectType +"'  from='"+ startFrom +"' total='"+totalRecords+"'/>" );        			
	        				
		if(objectList != null) {
			MRXMLUtils.addChildrenToElement(MRXMLUtils.getElementXpath(ObjectListDoc, "//ObjectList"),objectList);			
		}
		
		return ObjectListDoc;
		
	}

	private Document getQueryList(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db,ServiceAgentFactoryI serviceAgentFactory,MRResourceFactoryI resourceFactory, int startFrom, int countToFetch, 
			String xpath, String sortBy, String sortOrder, String detail,
			String queryFile) throws Exception {
		long dbtimeout = MRXMLDBUtils.TIME_OUT;
		
		String objectQuery = FileUtils.readFileResource(queryFile, request.getClass());
		String objectListQuery = FileUtils.readFileResource(SessionServiceConst.OBJECTLISTXQUERY, request.getClass());
		String query = objectQuery + objectListQuery;
		query = query.replace(SessionServiceConst.STARTFROM, String.valueOf(startFrom));
		query = query.replace(SessionServiceConst.COUNTTOFETCH, String.valueOf(countToFetch));
		query = query.replace(SessionServiceConst.XPATHAND, xpath);
		query = query.replace(SessionServiceConst.SORTBY, sortBy);
		query = query.replace(SessionServiceConst.SORTORDER, sortOrder);
		query = query.replace(SessionServiceConst.DETAIL, detail);
		Map<String, String> pemXpathList =  PolicyAdminServiceUtils.getPEMXPathListForReadQuery(query, request, db, log, xmpp, serviceAgentFactory, resourceFactory); 
		String queryResult = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, pemXpathList, null);
		return MRXMLUtils.stringToDocument(queryResult);
	}

}
