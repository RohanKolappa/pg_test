package com.ipvs.common.service;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;


public class AuditEventUtils {

	private  static final String AUDIT_EVENT_MAPPING_XML = "/com/ipvs/common/service/AuditEventMapping.xml";
	private static Document auditEventToDbObjectMappingDoc = null;

	public static String getAuditEventLevel(MRRequest request) throws MRException,Exception {
		if(request.getException()!=null)
			return AuditEventLogger.LEVEL_ERROR;
		return AuditEventLogger.LEVEL_INFO;
	}

	public static String getAuditEventStatus(MRRequest request) throws MRException,Exception {
		if(request.getException()!=null)
			return AuditEventLogger.STATUS_FAILURE;
		return AuditEventLogger.STATUS_SUCCESS;
	}

	public static String getAuditEventErrorDescription(MRRequest request) throws MRException,Exception {
		if(request.getException()==null) {
			return "";
		}
		if(request.getException().getCode() == MRExceptionCodes.SYSTEMERROR) {
			return AuditEventLogger.ERROR_TOKEN+"System Error";
		}
		return  AuditEventLogger.ERROR_TOKEN+request.getException().getDescription();

	}

	public static AuditEvent getAuditEventObjectForGivenObjectAndAction(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String status,
			String level, String errorDescription, String dbObjectName,	String action) throws MRException,Exception {
		return getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), request.getMessage().getDocument(), request.getResult(), request.getOperatorGroups(), status, level, errorDescription, dbObjectName, action);
	}
	
	public static Document getAuditEventMappingDoc(MRLog log) throws MRException,Exception {
		if(auditEventToDbObjectMappingDoc == null) {
			auditEventToDbObjectMappingDoc = MRXMLUtils.loadXMLResource(AUDIT_EVENT_MAPPING_XML, log);
		}
		return auditEventToDbObjectMappingDoc;
	}
	
	public static boolean logAduitEventForGivenRequest(String requestName,MRLog log) throws MRException,Exception {
	   if(requestName==null ||  requestName.isEmpty()) {
		   return false;
		}
	   Element mappingEl = MRXMLUtils.getElementXpath(getAuditEventMappingDoc(log), "//EventMapping[RequestList/Request='"+requestName+"']");
	   if(mappingEl ==null) {
		   return false;
	   }
	   return true;
	}
	
	public static AuditEvent getAuditEventObjectForGivenObjectAndAction(MRLog log,String userJID,Document requestDoc,Document resultDoc, Element operatorEL,String status,
			String level, String errorDescription, String dbObjectName,	String action) throws MRException,Exception {
		return getAuditEventObjectFromMappingFile(log, userJID, requestDoc, resultDoc, operatorEL, status, level, errorDescription, dbObjectName, action, "");
	}
	
	public static AuditEvent getAuditEventObjectForGivenRequest(MRLog log,String userJID,Document requestDoc,Document resultDoc, Element operatorEL,String status,
			String level, String errorDescription,String requestName) throws MRException,Exception {
		return getAuditEventObjectFromMappingFile(log, userJID, requestDoc, resultDoc, operatorEL, status, level, errorDescription, "", "", requestName);
	}
	
	public static AuditEvent getAuditEventObjectFromMappingFile(MRLog log,String userJID,Document requestDoc,Document resultDoc, Element operatorEL,String status,
			String level, String errorDescription, String dbObjectName,	String action,String requestName) throws MRException,Exception {
		try {
			
			String eventMappingXpath ="//EventMapping[@action='"+action+"' and @objectType='"+dbObjectName+"']";
			if(requestName!=null && !requestName.isEmpty()) {
				eventMappingXpath ="//EventMapping[RequestList/Request='"+requestName+"']";
			}
			
			List<?> eventMappingList = MRXMLUtils.getListXpath(getAuditEventMappingDoc(log), eventMappingXpath);
			if(eventMappingList.isEmpty()) {
				return null;
			}
			Element eventMappingEl = null;
			if(eventMappingList.size() > 1) {
				for(Object object:eventMappingList) {
					Element element = (Element) object;
					String xPath = MRXMLUtils.getAttributeValue(element, "xPath","");	
					if(!xPath.isEmpty()) {
						if(MRXMLUtils.getElementXpath(requestDoc, xPath) != null || MRXMLUtils.getElementXpath(resultDoc, xPath)!=null) {
							eventMappingEl = element;
						}
					}
				}
			} else {
				eventMappingEl = (Element) eventMappingList.get(0);
			}
			
			if(eventMappingEl == null) {
				return null;
			}
			
			String type  = MRXMLUtils.getAttributeValue(eventMappingEl, "eventType","");
			String name  = MRXMLUtils.getAttributeValue(eventMappingEl, "eventName","");
			String target  = "";
			Element targetGroupsEl =null;
			String descriptionText = MRXMLUtils.getAttributeValue(eventMappingEl, "descriptionText","");
			String targetXPath  = MRXMLUtils.getAttributeValue(eventMappingEl, "targetXPath","");
			if(resultDoc!=null && !targetXPath.isEmpty()) {
				List<?> targetList  = MRXMLUtils.getValuesXpath(resultDoc, targetXPath);
				if(targetList !=null && !targetList.isEmpty()) {
					target = StringUtils.join(targetList,AuditEventLogger.SEPERATOR);
					if(targetList.size()==1) {
						targetGroupsEl = MRXMLUtils.getElementXpath(resultDoc, "//Groups");
					}
				}
			} 
			
            if(target.isEmpty() && !targetXPath.isEmpty()) {
				target  = MRXMLUtils.getValueXpath(requestDoc, targetXPath, "");
			}
			
			List<?> descriptionTextList = MRXMLUtils.getListXpath(eventMappingEl, "./DescriptionTextList/DescriptionText");
			if(!descriptionTextList.isEmpty()) {
				StringBuffer sbuffer = new StringBuffer();
				String delim = "";
				for(Object object1:  descriptionTextList) {
					Element element = (Element) object1;
					String xPath = MRXMLUtils.getAttributeValue(element, "xPath","");
					List<String> valueList = MRXMLUtils.getValuesXpath(resultDoc, xPath);
					if(valueList != null  && !valueList.isEmpty()) {
					    String valueString = StringUtils.join(valueList, " ");
					    String descriptionName= MRXMLUtils.getAttributeValue(element, "name","");
						sbuffer.append(delim + descriptionName + valueString);
						delim =" ";
					}
				}
				descriptionText += sbuffer.toString();
			}
			if(!errorDescription.isEmpty()) {
				descriptionText +=errorDescription;
			}
			return  new  AuditEvent.Builder(type,name, userJID, level)
			.target(target).status(status).descriptionText(descriptionText).operatorEl(operatorEL).targetEl(targetGroupsEl).build();
		} catch (Exception e) {
			log.addInfo("Exception occur during logging event. Description:="+MRException.getStackTrace(e));
			e.printStackTrace();
		}	
		return null;
	}
}
