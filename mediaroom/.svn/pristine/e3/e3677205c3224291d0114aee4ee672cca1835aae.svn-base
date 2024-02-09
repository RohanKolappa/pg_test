package com.ipvs.sessionservice.impl;

import java.text.SimpleDateFormat;
import java.util.Date;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class SessionAuditEventUtils {


	public static String getAuditeEventNameForGivenConnectionType(String connectionType) {
		if(connectionType.equals(SessionServiceConst.VIEW)){
			return AuditEventLogger.NAME_VIEW;
		}else if(connectionType.equals(SessionServiceConst.PLAYBACK)){
			return AuditEventLogger.NAME_PLAYBACK;
		}else if(connectionType.equals(SessionServiceConst.RECORD)){
			return AuditEventLogger.NAME_RECORD;
		}
		return null;
	}

	public static Element getSourceEntityElementFromConnectionElement(Document sessionDoc,Element connectionElement,MRRequest request,MRLog log) throws MRException,Exception {
		String type = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.TYPE_ATTRIBUTE,"");		
		String srcElementName = SessionServiceConst.PORT;
		if(type.equals(SessionServiceConst.PLAYBACK)){
			srcElementName = SessionServiceConst.FILE;
		}

		return MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.SRCENTITYID,"")+"']/"+srcElementName);
	}

	public static AuditEvent  getSourceBasedAuditEventUsingConnectionElement(Document responseDoc,Element connectionElement,MRRequest request,MRLog log) throws MRException,Exception {		

		Document sessionDoc = MRXMLUtils.getDocumentXpath(responseDoc, "//Session");
		String type = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.TYPE_ATTRIBUTE,"");		

		String eventName = AuditEventLogger.NAME_SOURCE;
		if(type.equals(SessionServiceConst.PLAYBACK)){
			eventName = AuditEventLogger.NAME_FILE;
		}

		Element srcElement = SessionAuditEventUtils.getSourceEntityElementFromConnectionElement(sessionDoc, connectionElement,  request, log);		

		return getAuditEventBasedOnSourceAndDestination(sessionDoc, connectionElement,  eventName, srcElement, request, log);
	}


	public static AuditEvent  getSourceBasedAuditEvent(Document responseDoc,Element connectionElement,String srcNID,MRRequest request,MRLog log) throws MRException,Exception {		
		Document sessionDoc = MRXMLUtils.getDocumentXpath(responseDoc, "//Session");
		String type = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.TYPE_ATTRIBUTE,"");
		String srcElementName = SessionServiceConst.PORT;
		String eventName = AuditEventLogger.NAME_SOURCE;
		if(type.equals(SessionServiceConst.PLAYBACK)){
			srcElementName = SessionServiceConst.FILE;
			eventName = AuditEventLogger.NAME_FILE;
		}

		Element srcElement = MRXMLUtils.getElementXpath(sessionDoc, "//"+srcElementName+"[@resourceNID='"+srcNID+"']");
		return getAuditEventBasedOnSourceAndDestination(sessionDoc, connectionElement, eventName, srcElement, request, log);
	}


	public static AuditEvent  getGroupBasedAuditEvent(Document responseDoc,Element connectionElement,String grpNID,MRRequest request,MRLog log) throws MRException,Exception {		
		Document sessionDoc = MRXMLUtils.getDocumentXpath(responseDoc, "//Session");
		String srcElementName = SessionServiceConst.GRPFILE;
		Element srcElement = MRXMLUtils.getElementXpath(sessionDoc, "//"+srcElementName+"[@resourceNID='"+grpNID+"']");
		if(connectionElement == null) {
			String srcEntityId = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//Entity[File/@mediaGroupId='"+MRXMLUtils.getAttributeValue(srcElement, SessionServiceConst.MEDIAGROUPID,"")+"']", SessionServiceConst.ID, "");
			connectionElement = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@srcEntityId='"+srcEntityId+"']");
		}
		return getAuditEventBasedOnSourceAndDestination(sessionDoc, connectionElement,  AuditEventLogger.NAME_GROUP,  srcElement, request, log);
	}


	public static AuditEvent  getAuditEventBasedOnSourceAndDestination(Document sessionDoc,Element connectionElement,String eventName,
			Element srcElement,MRRequest request,MRLog log) throws MRException,Exception {		
		String status = getStatus(connectionElement);  
		String level = getLevel(connectionElement);	
		String errorDescription = getErrorDescriptionString(connectionElement);
		return getAuditEventBasedOnSourceAndDestination(sessionDoc, connectionElement, eventName, srcElement, status, level, errorDescription, request, log);
	}
	public static AuditEvent  getAuditEventBasedOnSourceAndDestination(Document sessionDoc,Element connectionElement,String eventName,
			Element srcElement,String status,String level,String errorDescription, MRRequest request,MRLog log) throws MRException,Exception {

		String connectionType = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.TYPE_ATTRIBUTE,"");		

		String eventType = SessionAuditEventUtils.getAuditeEventNameForGivenConnectionType(connectionType);
		if(eventType == null) {
			return null;
		}
		
		String srcTitle = getSourceTitle(srcElement);		
		String destinationDescString = getDestinationDescriptionString(sessionDoc, connectionType, connectionElement);  
		String sessionNameDescString = getSessionNameDescriptionString(sessionDoc);	
		String description =  sessionNameDescString +" " + destinationDescString;
		if(!errorDescription.isEmpty()) {
			description += " "+errorDescription;
		}		
		Element groupEL = MRXMLUtils.getElementXpath(sessionDoc, "//Groups");

		return  new AuditEvent.Builder(eventType, eventName, request.getUserJID(), level).target(srcTitle).status(status).
		descriptionText(description).operatorEl(request.getOperatorGroups()).targetEl(groupEL).build();
	}
	
	public static String getSourceTitle(Document sessionDoc,Element connectionElement,MRRequest request,MRLog log) throws MRException,Exception {
		Element srcElement = getSourceEntityElementFromConnectionElement(sessionDoc, connectionElement, request, log);
		return getSourceTitle(srcElement);
	}

	public static String getSourceTitle(Element srcElement) throws MRException,Exception {		
		String srcTitle = MRXMLUtils.getAttributeValue(srcElement, SessionServiceConst.RESOURCETITLE,"");
		String dateCreated = MRXMLUtils.getAttributeValue(srcElement,SessionServiceConst.DATE_CREATED,"");
		if(!dateCreated.isEmpty()) {
			long timeStamp = Utils.getLongValue(dateCreated, -1);
			if(timeStamp != -1) {
				SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yy HH:mm:ss");		
				String dateCreatedTime = simpleDateFormat.format(new Date(timeStamp));
				srcTitle += " - "+dateCreatedTime;
			}
		}   
	
		return srcTitle;
	}

	public static String getDestinationDescriptionString(Document sessionDoc,String connectionType,Element connectionElement) throws MRException,Exception {
		String dstElementName = SessionServiceConst.PORT;
		if(connectionType.equals(SessionServiceConst.RECORD)){
			dstElementName = SessionServiceConst.FILE;
		}

		Element dstElement = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.DSTENTITYID,"")+"']/"+dstElementName);
		String dstTitle = MRXMLUtils.getAttributeValue(dstElement, SessionServiceConst.RESOURCETITLE,"");    	
		if(connectionType.equals(SessionServiceConst.RECORD)){
			dstTitle = " File - "+ dstTitle;
		} else {
			dstTitle = " Destination - "+ dstTitle;
		}

		return dstTitle;

	}

	public static String getSessionNameDescriptionString(Document sessionDoc) throws MRException,Exception {
		String sessionName = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.NAME,"");		
		return "Session - " + sessionName ;
	}

	public static String getErrorDescriptionString(Element connectionElement) throws MRException,Exception {
		String errorDescription = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.ERROR_DATA.toLowerCase(),"");
		if(!errorDescription.isEmpty()){
			return AuditEventLogger.ERROR_TOKEN+errorDescription;			
		}
		return errorDescription;
	}

	public static String getLevel(Element connectionElement) throws MRException,Exception {
		String errorDescription = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.ERROR_DATA.toLowerCase(),"");
		if(!errorDescription.isEmpty()){
			return AuditEventLogger.LEVEL_ERROR;			
		}
		return AuditEventLogger.LEVEL_INFO;
	}

	public static String getStatus(Element connectionElement) throws MRException,Exception {
		String errorDescription = MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.ERROR_DATA.toLowerCase(),"");
		if(!errorDescription.isEmpty()){
			return AuditEventLogger.STATUS_FAILURE;			
		}
		return AuditEventLogger.STATUS_SUCCESS;
	}

	public static AuditEvent getSessionAuditEventObject(Document responseDoc,String eventAction,MRRequest request,String status,String level,MRLog log,String errorDesString) throws MRException,Exception {
		Document sessionDoc = MRXMLUtils.getDocumentXpath(responseDoc, "//Session");
		Element groupEL = MRXMLUtils.getElementXpath(sessionDoc, "//Groups");
		String sessionName = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.NAME,"");
		return  new AuditEvent.Builder(AuditEventLogger.SESSION_EVENT, eventAction, request.getUserJID(), level).target(sessionName).status(status).
		descriptionText(errorDesString).operatorEl(request.getOperatorGroups()).targetEl(groupEL).build();
	}


}
