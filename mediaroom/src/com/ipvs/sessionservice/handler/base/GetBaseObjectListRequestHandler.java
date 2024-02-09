package com.ipvs.sessionservice.handler.base;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import javax.ws.rs.core.MultivaluedMap;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;

public class GetBaseObjectListRequestHandler {
	
	/**
	 * 
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param serviceAgentFactory
	 * @param resourceFactory
	 * @param requestElement
	 * @param viewObjectType : session object name 
	 * @param modelObjectType : db object name 
	 * @param sortBy
	 * @param listOfWhereXPath : Used for custom where Xpath
	 * @param xqueryFileName :
	 * @param customeTokenMappingTable: Used for to replace the custom tokens in given query 
	 * @param pemObjectList:To be used if query has native db collection clause instead of #OBJ_ObjectType_#OBJ to go through the pem check 
	 * @return
	 * @throws Exception
	 */
	public Document getObjectListRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, Element requestElement, 
			String viewObjectType,String modelObjectType,String sortBy, List<String> listOfWhereXPath, String xqueryFileName, Hashtable<String, String> customeTokenMappingTable,List<String> pemObjectList) throws Exception {
		String startFromStr = MRXMLUtils.getAttributeValue(requestElement, "startFrom", "1");
		int startFrom = Utils.getIntValue(startFromStr,1);
		if (startFrom == 0){
			throw new MRException(SessionServiceExceptionCodes.INVALID_STARTFROM, "Invalid StartFrom");
		}
		//startFrom = startFrom+ 1;
		String countToFetchStr = MRXMLUtils.getAttributeValue(requestElement, "countToFetch", "1000");
		int countToFetch = Utils.getIntValue(countToFetchStr, 1000);
		//String sortBy = MRXMLUtils.getAttributeValue(requestElement, "sortBy", "");
		String sortOrder = MRXMLUtils.getAttributeValue(requestElement, "sortOrder", "descending");
		String detail = MRXMLUtils.getAttributeValue(requestElement, "selectFormat", "BASIC");
		String whereNID = MRXMLUtils.getAttributeValue(requestElement, "whereNID", "");
		String whereTagExpr = MRXMLUtils.getAttributeValue(requestElement, "whereTagExpr", "");
		String whereDateRange = MRXMLUtils.getAttributeValue(requestElement, "whereDateRange", "");
		String whereTextSubStr = MRXMLUtils.getAttributeValue(requestElement, "whereTextSubStr", "");
		String searchTextSubStrIn = MRXMLUtils.getAttributeValue(requestElement, "searchTextSubStrIn", "");
		String wherePermissions = MRXMLUtils.getAttributeValue(requestElement, "wherePermissions", "");
		Document resultDoc=  null;
		resultDoc = GetObjectUtils.getQueryList(request, xmpp, log, db,serviceAgentFactory,resourceFactory, startFrom, countToFetch, 
				sortBy, sortOrder, detail, whereDateRange, whereNID, whereTagExpr, whereTextSubStr, searchTextSubStrIn, wherePermissions,listOfWhereXPath, xqueryFileName, viewObjectType,modelObjectType,customeTokenMappingTable,pemObjectList);
		String requestName = requestElement.getName();
		Element responseEL = MRXMLUtils.newElement(requestName);
		if ((resultDoc != null)) {
			responseEL = resultDoc.getRootElement();
			responseEL.setName(requestName);
		}
		return request.createResponseData(responseEL);
	}
	
	public Document getObjectListRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, Element requestElement, String viewObjectType,String sortBy, List<String> listOfWhereXPath, String xqueryFileName) throws Exception {
		return getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, requestElement, viewObjectType,"",sortBy, listOfWhereXPath, xqueryFileName,null,null);
	}
	
	
	public Document getObjectListRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, Element requestElement, String viewObjectType,String modelObjectType,String sortBy, List<String> listOfWhereXPath, String xqueryFileName) throws Exception {
		return getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, requestElement, viewObjectType,modelObjectType,sortBy, listOfWhereXPath, xqueryFileName, null,null);
	}
	
	public Document getObjectListRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, 
			String viewObjectType, HashMap<String, String> queryParams, String xQueryFileName, List<String> listOfWhereXPath, String modelObjectType, String requestName, boolean forceXMPP) throws Exception {
		return GetObjectUtils.getQueryList(request, xmpp, log, db, serviceAgentFactory, resourceFactory, viewObjectType, queryParams, xQueryFileName, listOfWhereXPath, modelObjectType, null, null, requestName, forceXMPP);
	}
	
	public HashMap<String, String> getHttpRequestQueryParams(MultivaluedMap<String, String> queryParams) throws Exception {
		HashMap<String, String> queryParamMap = new HashMap<String, String>();
		
		if(queryParams == null) {
			return null;
		}
		
		for (String param : queryParams.keySet()) {
			if(!queryParams.get(param).get(0).isEmpty())
				queryParamMap.put(param, StringUtils.join(queryParams.get(param), ","));
		}
		
		return queryParamMap;
	}
	
	public HashMap<String, String> getXmppRequestQueryParams(Element getPortElement) throws Exception {
		HashMap<String, String> queryParamMap = new HashMap<String, String>();
		List<?> attrList = MRXMLUtils.getAttributeList(getPortElement);
        if(attrList != null) {
        	for (Iterator<?> i = attrList.iterator(); i.hasNext();) {
                Attribute attrEL = (Attribute) i.next();
                if(attrEL != null) {
                	String name = attrEL.getName();
                    String val = attrEL.getValue();
                    if(name != null && val != null && !val.isEmpty())
                    	queryParamMap.put(name, val);
                }
            }
        }
    	return queryParamMap;
	}
	
	class SubscribeUser implements Runnable{
		private Element requestElement ;
		private MRRequest request;
		private MRLog log;
		private MRDB db;
		private XMPPI xmpp;
		private ServiceAgentFactoryI serviceAgentFactory;
		private MRResourceFactoryI resourceFactory;
		private String viewObjectType;
		private String modelObjectType;
		private String whereTypeXpath;
		
		private Document resultDoc;
		public SubscribeUser(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, Element requestElement, String viewObjectType,String modelObjectType, String whereTypeXpath, Document resultDoc) {
			this.request= request;
			this.xmpp= xmpp;
			this.log= log;
			this.db=db;
			this.serviceAgentFactory =serviceAgentFactory;
			this.resourceFactory=resourceFactory;
			this.requestElement =requestElement;
			this.viewObjectType=viewObjectType;
			this.modelObjectType = modelObjectType;
			this.whereTypeXpath = whereTypeXpath;
			this.resultDoc = resultDoc;
		}
		@Override
		public void run() {
			try {
				String subscriptionScope = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.SUBSCRIPTION_SCOPE,"");
				//if the scope is all nodes then get all the nodes from database  which user can access based on access policies and subscribe user on result document.
				//This will cause some performance issue. need to revisit - Padmaja
				if(subscriptionScope.equals(SessionServiceConst.ALLNODES)) {
					String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + viewObjectType + SessionServiceConst.XQUERYXML;
					HashMap<String, String> queryParams = new HashMap<String, String>();
					//basic format is enough to get all the nodes
					queryParams.put("selectFormat", SessionServiceConst.SELECTFORMAT_BASIC);
					//type can not consider as where clause. Its kind of request in our system. 
					List<String> xpathList = new ArrayList<String>();
					if(whereTypeXpath != null && !whereTypeXpath.isEmpty())
						xpathList.add(whereTypeXpath);
					Document responseDoc = getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, viewObjectType, 
							new HashMap<String, String>(), xqueryFileName, xpathList, modelObjectType, MRRequest.getRequestRootName(request.getRequestName()), true);
					resultDoc = responseDoc;
					log.addInfo("SubScribeUser: User subscription scope is ALLNODES. Subscribe user:"+request.getUserJID() + " for viewObject:="+viewObjectType+" modelObject:="+modelObjectType,MRLog.INFO,MRLog.NOTIFY);
				}
				
				GetObjectUtils.subscribeUser(requestElement, resultDoc, viewObjectType, request, serviceAgentFactory);
				
			} catch (Exception e) {
				e.printStackTrace();
			}

		}

	}

	protected void subscribeUser(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, Element requestElement, String viewObjectType,String modelObjectType, String whereTypeXpath, Document resultDoc) throws Exception {
		String subscriptionId = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.SUBSCRIPTION_ID,"");
		if(subscriptionId.isEmpty()) {
			return;
		}
		new Thread(new SubscribeUser(request, xmpp, log, db, serviceAgentFactory, resourceFactory, requestElement, viewObjectType, modelObjectType, whereTypeXpath, resultDoc)).start();
		
	}
	
	public void fillConnectionStateInfoInResponseDoc(Document resultDoc, MRRequest request, XMPPI xmpp, MRDB db, MRLog log,
			ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory,String selectFormatType, String objectType, String msIPAddress,String authScheme,String dstTemplateTitle) throws MRException,Exception {

		/*
		   XML Format
		   <ConnectionStatus>
				<Session id='' name=''>
					<Connection id='' name='' state=''  type='LIVE/PLAYBACK/RECORD/PVR' 
					 srcNID='' srcTitle='' dstNID='' dstTitle='' windowId='' canvasId=''/>
				</Session>
			</ConnectionStatus>
		 */
		Document sessionListDoc = null;
		Boolean includeMediaRoomInfo = false;
		int startFrom = 1;
		int countToFetch = -1;
		String srcXpath="";
		String dstXpath="";
		
		List<String> resourceNIDList = MRXMLUtils.getValuesXpath(resultDoc, "//"+objectType+"/@NID");
		if(resourceNIDList == null)
			return;
		
		if(SessionServiceConst.PORT.equalsIgnoreCase(objectType)){
			srcXpath = "../../EntityList/Entity[@type='SrcPort' and Port/@resourceNID[.!='' and contains('"+StringUtils.join(resourceNIDList, ",")+"',.)]]/@id";
			dstXpath = "../../EntityList/Entity[@type='DstPort' and Port/@resourceNID[.!='' and contains('"+StringUtils.join(resourceNIDList, ",")+"',.)]]/@id";
		}else{
			srcXpath = "../../EntityList/Entity[@type='PlyFile' and File/@resourceNID[.!='' and contains('"+StringUtils.join(resourceNIDList, ",")+"',.)]]/@id";
			dstXpath = "../../EntityList/Entity[@type='DstPort' and File/@resourceNID[.!='' and contains('"+StringUtils.join(resourceNIDList, ",")+"',.)]]/@id";
		}
		XMPPTransaction.startBlock("getSessionListWithMediaRoomInfo");
		sessionListDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, startFrom, countToFetch, includeMediaRoomInfo, "//Session/ConnectionList/Connection[@srcEntityId="+srcXpath+" or @dstEntityId="+dstXpath+"]");
		XMPPTransaction.endBlock("getSessionListWithMediaRoomInfo");
		
		XMPPTransaction.startBlock("getSessionDocWithMediaRoomInfo");
    	try {
    		List<?> connectionList = MRXMLUtils.getListXpath(sessionListDoc, "//Session/ConnectionList/Connection[@srcEntityId="+srcXpath+" or @dstEntityId="+dstXpath+"]");
    		if(connectionList == null) {
				return;
			}
    		
			for(Object object:connectionList) {
				Element sessionConnEL = (Element) object;
				Element streamEl = null;
				Element mediaSourceEl = null;
				Element sessionEL = null;
				Element sessionInfoEL = null;
				Element srcConnectionStatusEL = null;
				Element dstConnectionStatusEL = null;
				
				sessionEL = sessionConnEL.getParent().getParent();
				
				String srcEntityId = MRXMLUtils.getAttributeValue(sessionConnEL, SessionServiceConst.SRCENTITYID, "");
				String dstEntityId = MRXMLUtils.getAttributeValue(sessionConnEL, SessionServiceConst.DSTENTITYID, "");
				String srcResourceNID = MRXMLUtils.getAttributeValueXpath(sessionEL, ".//Entity[@id='"+srcEntityId+"']/"+objectType ,SessionServiceConst.RESOURCENID, "");
				String dstResourceNID = MRXMLUtils.getAttributeValueXpath(sessionEL, ".//Entity[@id='"+dstEntityId+"']/"+objectType ,SessionServiceConst.RESOURCENID, "");
				
				srcConnectionStatusEL = MRXMLUtils.getElementXpath(resultDoc, "//"+objectType+"[@NID='"+srcResourceNID+"']/ConnectionStatus");
				if(srcConnectionStatusEL == null) {
					srcConnectionStatusEL = MRXMLUtils.newElement(SessionServiceConst.CONNECTION_STATUS);
					MRXMLUtils.addElementXpath(resultDoc, "//"+objectType+"[@NID='"+srcResourceNID+"']", srcConnectionStatusEL);
				}
				dstConnectionStatusEL = MRXMLUtils.getElementXpath(resultDoc, "//"+objectType+"[@NID='"+dstResourceNID+"']/ConnectionStatus");
				if(dstConnectionStatusEL == null) {
					dstConnectionStatusEL = MRXMLUtils.newElement(SessionServiceConst.CONNECTION_STATUS);
					MRXMLUtils.addElementXpath(resultDoc, "//"+objectType+"[@NID='"+dstResourceNID+"']", dstConnectionStatusEL);
				}
				
				String sessionId = MRXMLUtils.getAttributeValueXpath(sessionEL, "./@id", "");
				
				Element srcSessionInfoEL = MRXMLUtils.getElementXpath(srcConnectionStatusEL, "./"+objectType+"SessionInfo[@id='"+sessionId+"']");
				Element dstSessionInfoEL = MRXMLUtils.getElementXpath(dstConnectionStatusEL, "./"+objectType+"SessionInfo[@id='"+sessionId+"']");
				if((srcSessionInfoEL == null) || (dstSessionInfoEL == null)){
					sessionInfoEL = AppServerAgentUtils.getResourceManager(serviceAgentFactory).generateSessionInfoElement(sessionEL, objectType);
					if(srcSessionInfoEL == null) {
						srcSessionInfoEL = (Element)sessionInfoEL.clone();
						MRXMLUtils.addElement(srcConnectionStatusEL, srcSessionInfoEL);
					}
					if(dstSessionInfoEL == null) {
						dstSessionInfoEL = (Element)sessionInfoEL.clone();
						MRXMLUtils.addElement(dstConnectionStatusEL, dstSessionInfoEL);
					}
				}
				
				Element connectionInfoEL = AppServerAgentUtils.getResourceManager(serviceAgentFactory).generateBasicConnectionInfoElement(sessionEL, sessionConnEL, request, log, db, objectType);
				
				if(selectFormatType.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO)) {
					String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
					Document roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
					String connID = MRXMLUtils.getAttributeValueXpath(sessionConnEL, "./@id", "");
					streamEl = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id='" + connID + "']");
					String mediaSourceNID = MRXMLUtils.getValueXpath(streamEl, ".//mediaSourceNID", "");
					mediaSourceEl = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@NID='" + mediaSourceNID + "']");
					connectionInfoEL = AppServerAgentUtils.getResourceManager(serviceAgentFactory).fillExtendedConnectionFormatInfo(connectionInfoEL,  streamEl, mediaSourceEl,sessionEL.getDocument(),connID, db, log);
				}
				
				if(srcSessionInfoEL != null) {
					MRXMLUtils.addElement(srcSessionInfoEL, (Element)connectionInfoEL.clone());
				}
				
				if(dstSessionInfoEL != null) {
					if(objectType.equals(SessionServiceConst.PORT)){
						String sourceNID = MRXMLUtils.getAttributeValue(connectionInfoEL, "srcNID");
						Element ApiKeyTableEl = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getAPIKey();
						String expireTime = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE, String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
						String timeStamp =String.valueOf(xmpp.getCurrentTimeMillis() + (Long.parseLong(expireTime) * 1000));


						String mediaType = ( MRXMLUtils.getAttributeValue(connectionInfoEL, SessionServiceConst.TYPE_ATTRIBUTE) == SessionServiceConst.VIEW ) ? "live" : "recording"; 
						String thumbnailURL = HttpAuthentication.generateThumbnailURL(sourceNID, mediaType, timeStamp, msIPAddress,authScheme, log, serviceAgentFactory, request,db,xmpp,resourceFactory,dstTemplateTitle);
						if (!thumbnailURL.isEmpty()) {
						   MRXMLUtils.addAttribute(connectionInfoEL,SessionServiceResourceConst.THUMBNAILURL_ATTRIBUTE,thumbnailURL);
						}
					}
					MRXMLUtils.addElement(dstSessionInfoEL, (Element)connectionInfoEL.clone());
				}
			}
			   		
			    	
    	}catch(Exception e) {
    		XMPPTransaction.endBlock("getSessionDocWithMediaRoomInfo");
    		throw e;
    	}
		XMPPTransaction.endBlock("getSessionDocWithMediaRoomInfo");
	
	}
	 
	
	protected String getDstTitle(Element requestEl, String selectFormat,MRRequest request) 
	{
		String dstPortTitle = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.DST_PORT_TITLE,"");
		if(dstPortTitle.isEmpty() & (selectFormat.equals(SessionServiceConst.SELECTFORMAT_BASICURL) || selectFormat.equals(SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO)
				|| selectFormat.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDURL ) || selectFormat.equals(SessionServiceConst.SELECTFORMAT_EXTENDEDCONNECTIONINFO) 
				|| selectFormat.equals(SessionServiceResourceConst.SELECTFORMAT_SNAPSHOTURL))){
			try {
				dstPortTitle = StringUtils.substringAfter(request.getUserJID(), "@localhost/");
				if(dstPortTitle == null) {
					return  "";
				} 				
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
		}
		return dstPortTitle;

	}
}
