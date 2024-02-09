package com.ipvs.sessionservice.impl;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.utils.Utils;
/**
 * Session Auth implementation 
 * @author padmaja
 *
 */
public class SessionAuthUtils {


	public static String getXAuthAgentJIDFromMediaSource(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element connEl,Element mediaSourceEl, Element mediaDstEl) throws MRException,Exception {

		String type = MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.TYPE_ATTRIBUTE);
		String xAuthAgentJID = MRXMLUtils.getValueXpath(mediaSourceEl, ".//xAuthAgentJID", "");
		
		// AuthCheck for VIEW and PVR connections
		if((!type.equals(SessionServiceConst.VIEW ) && !(type.equals(SessionServiceConst.PVR)))
				|| xAuthAgentJID.isEmpty()){
			return "";
		}
		
		String dstResourceNID = MRXMLUtils.getValueXpath(mediaDstEl, ".//destPortNID", "");	
		String srcResourceNID = MRXMLUtils.getValueXpath(mediaSourceEl, ".//sourcePortNID", "");

		boolean isAuthPolicyEnabled = StreamPolicyUtils.isAuthPolicyEnabled(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstResourceNID, srcResourceNID);
		if (isAuthPolicyEnabled){
			return xAuthAgentJID;
		} 		
		return "";
	}
	
	
	public static void deleteAuthEntityIfExists(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element connectionElement,String roomNID,String sessionId,String connId) throws MRException,Exception {		
		
		String authEntityId = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.AUTH_ENTITY_ID,"");
		if(authEntityId.isEmpty()) {
			return;
		}
		String xpath = "//Connection[@id!='"+connId+"' and @authEntityId='"+authEntityId+"']";
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, xpath);
		if(connectionList.isEmpty()){
			AppServerAgentUtils.getSessionManager(serviceAgentFactory).deleteEntity(roomNID, authEntityId);
			log.addInfo("SessionAuthUtils: Delete Auth Entity. Id="+authEntityId,MRLog.INFO,MRLog.NOTIFY);
		}		
	}

	public static String addAuthEntityIfNotExists(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String xAuthAgentJID,String sessionID,String roomNID) throws MRException,Exception {
		
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
		
		Element authEntityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@type='AuthClient' and User/@userJID='"+xAuthAgentJID+"']");
		if(authEntityEL != null) {
			return MRXMLUtils.getAttributeValue(authEntityEL, SessionServiceConst.ID,"");
		}

		Element xAuthInfoElement = MediaRoomServiceUtils.getOperator(MRRequest.createRequest(xAuthAgentJID), log, db);
		String username = MRXMLUtils.getValueXpath(xAuthInfoElement, ".//User/Username", "");
		String userNID = MRXMLUtils.getAttributeValueXpath(xAuthInfoElement, ".//User",SessionServiceConst.NID, "");
		String resourceNID = MRXMLUtils.getAttributeValueXpath(xAuthInfoElement, ".//AssetResource",SessionServiceConst.NID, "");
		String resourceTitle =  MRXMLUtils.getValueXpath(xAuthInfoElement, ".//AssetResource/Info/Title", "");
		authEntityEL = createAuthEntity(log, resourceNID, resourceTitle, username, userNID, xAuthAgentJID);
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).addEntity(roomNID, authEntityEL);	
		String authEntityId = MRXMLUtils.getAttributeValue(authEntityEL, SessionServiceConst.ID,"");
		log.addInfo("SessionAuthUtils: Add Auth Entity. Id="+authEntityId +" JID:="+xAuthAgentJID,MRLog.INFO,MRLog.NOTIFY);
		return authEntityId;
	}

	public static Element createAuthEntity(MRLog log,String xAuthResourceNID, String xAuthResourceTitle, String username, String userNID,String xAuthAgentJID)  throws MRException,Exception {
		Document authInstanceDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.AUTHINSTANCE, log);
		Element authEntityEL = (Element) MRXMLUtils.getElement(authInstanceDoc, SessionServiceConst.ENTITY).detach();
		MRXMLUtils.setAttributeValue(authEntityEL, SessionServiceConst.ID, Utils.getUUID());
		MRXMLUtils.setAttributeValue(authEntityEL, SessionServiceConst.NAME, xAuthResourceTitle);
		Element entityUserEl =MRXMLUtils.getElement(authEntityEL, SessionServiceConst.USER);
		MRXMLUtils.setAttributeValue(entityUserEl, SessionServiceConst.USER_NAME_ATTRIBUTE,username);
		MRXMLUtils.setAttributeValue(entityUserEl, SessionServiceConst.USER_NID,userNID);
		Element entityAuthCtrlEL =MRXMLUtils.getElement(authEntityEL, SessionServiceConst.AUTH_CTRL);
		MRXMLUtils.setAttributeValue(entityAuthCtrlEL, SessionServiceConst.RESOURCENID,xAuthResourceNID);
		MRXMLUtils.setAttributeValue(entityAuthCtrlEL, SessionServiceConst.RESOURCETITLE,xAuthResourceTitle);					
		MRXMLUtils.setAttributeValue(entityUserEl, SessionServiceConst.USERJID,xAuthAgentJID);	
		return authEntityEL;
	}

	public static void sendAuthRequiredNotification(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element connectionEl, String sessionId, String roomNID)  throws MRException,Exception {
		
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		
		String authEntityId = "";
		Document authRequiredTemplateDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.AUTHORIZATIONREQUIREDTEMPLATE, log);		
		Element authEntityEL = null;
		String authJID = "";

		authEntityId = MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.AUTH_ENTITY_ID);
		authEntityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+authEntityId+"']");
		authJID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(authEntityEL, SessionServiceConst.USER), SessionServiceConst.USERJID,"");
		if(authJID.isEmpty()) {
			MRXMLUtils.setAttributeValue(connectionEl, SessionServiceConst.ERROR_CODE, String.valueOf(SessionServiceExceptionCodes.AUTH_USER_JID_NOT_FOUND));
			MRXMLUtils.setAttributeValue(connectionEl, SessionServiceConst.ERROR_DATA,"Auth User JID Not found");
			return;
		}
		
		//update connection 
		Document authRequiredDoc = (Document) authRequiredTemplateDoc.clone();
		
		//update connection info
		Element connectionInfoEl = AppServerAgentUtils.getResourceManager(serviceAgentFactory).generateAuthConnectionInfoElement(request, xmpp, log, db, connectionEl, sessionId, sessionDoc, roomNID);
		MRXMLUtils.getElementXpath(authRequiredDoc, "//"+SessionServiceConst.AUTHCONNECTIONINFO).detach();
		MRXMLUtils.addElement(authRequiredDoc.getRootElement(),connectionInfoEl);

		//send notification
		SessionServiceUtils.sendSessionMessageNotification(xmpp, authJID, authRequiredDoc);
	}
}
