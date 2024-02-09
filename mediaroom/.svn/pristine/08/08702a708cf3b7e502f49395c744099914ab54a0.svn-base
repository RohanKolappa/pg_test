package com.ipvs.common.service;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;

public class AuditEventLogger extends AsyncThreadBase{

	public static final String AUDIT_EVENT_LOG_XML = "/com/ipvs/common/service/AuditEventLog.xml";

	private static final String TYPE = "type";
	private static final String NAME = "name";
	private static final String LEVEL = "level";
	private static final String DATE = "date";
	private static final String TIME = "time";
	private static final String USER = "user";
	private static final String RESOURCE = "resource";
	private static final String USERGROUPS = "userGroups";
	private static final String RESOURCEGROUPS = "resourceGroups";
	private static final String TARGET = "target";
	private static final String TARGETGROUPS = "targetGroups";
	private static final String DESCRIPTIONTEXT = "text";
	private static final String DESCRIPTION = "Description ";
	private static final String STATUS = "status";
	private static final boolean logEventUsingDoc = false;
	
	public static final String STATUS_SUCCESS= "SUCCESS";
	public static final String STATUS_FAILURE= "FAILURE";
	public static final String LEVEL_INFO ="info";
	public static final String LEVEL_ERROR ="error";
	public static final String LEVEL_WARNING ="warning";
	
	public static final String SYSTEM_EVENT = "System";
	public static final String MEDIA_EVENT = "Media";
	public static final String SESSION_EVENT = "Session";
	public static final String SWITCH_EVENT = "Switch";
	public static final String VIEW_EVENT = "View";
	public static final String PLAYBACK_EVENT = "Playback";
	public static final String RECORD_EVENT = "Record";
	
	public static final String NAME_LOGIN = "Login";
	public static final String NAME_LOGOUT = "Logout";
	public static final String NAME_UPGRADE = "Upgrade";
	public static final String NAME_RESTORE_DATABASE = "RestoreDatabase";
	public static final String NAME_BACKUP_DATABASE = "BackupDatabase";
	public static final String NAME_SEARCH = "Search";
	public static final String NAME_MODIFY = "Modify";
	public static final String NAME_DELETE = "Delete";
	public static final String NAME_START = "Start";
	public static final String NAME_STOP = "Stop";
	public static final String NAME_VIEW = "View";
	public static final String NAME_PLAYBACK = "Playback";
	public static final String NAME_RECORD = "Record";
	public static final String NAME_SWITCH= "Switch";
	public static final String NAME_SOURCE= "Source";
	public static final String NAME_FILE= "File";
	public static final String NAME_GROUP= "Group";
	
	public static final String SEPERATOR = ":";
	public static final String ERROR_TOKEN=" ERROR"+SEPERATOR;

	private static AuditEventLogger auditEventLogger = null;
	private AuditEventLogger(){
		super();
	}

	public static AuditEventLogger getInstance() {
		if(auditEventLogger == null) {
			auditEventLogger = new AuditEventLogger();
		}
		return auditEventLogger;
	}


	public void addAuditEventToQueue(AuditEvent auditEvent) throws MRException,Exception {
		this.addRequest(auditEvent);
		MRLog.getInstance(MRLog.CONSOLE_DAEMON).addInfo("Add AuditEvent To Queue. userJID:="+auditEvent.userJID+" type:="+auditEvent.type +" name:="+auditEvent.name,MRLog.INFO,MRLog.NOTIFY);
	}

	protected void handleRequest(Object object) throws MRException,Exception {
		logEvent((AuditEvent)object);
	}


	private void logEvent(AuditEvent auditEvent) throws Exception {
		String user = "";
		String userJID = auditEvent.userJID;

		if (!userJID.isEmpty())
		{
			int index = userJID.indexOf("@");
			if(index != -1)
				user = userJID.substring(0, index);
			else user =  userJID;
		}
		String resource = ServiceAgentUtils.getAgentName(userJID);

		SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yy HH:mm:ss");		
		String currentDateTime = simpleDateFormat.format(new Date());
		int index = currentDateTime.indexOf(" ");

		String date = currentDateTime.substring(0, index);    	
		String time = currentDateTime.substring(index + 1);
	
		String userGroups = getGroupsString(auditEvent.operatorEl, ".//User/.//Groups/Tag/@Value");
		String resourceGroups = getGroupsString(auditEvent.operatorEl, ".//Resource/.//Groups/Tag/@Value");
		String targetGroups = getGroupsString(auditEvent.targetEl, ".//Tag/@Value");
		
		String logStr = "";		
		if(logEventUsingDoc) {
			logStr = generateLogStringFromDoc(date,time,user,resource,auditEvent,userGroups,resourceGroups,targetGroups);
		} else {
			logStr = generateLogString(date,time,user,resource,auditEvent,userGroups,resourceGroups,targetGroups);
		}

		// Replace all occurrences of newline character with empty string  
		// Required for extracting portion of text from the log file during FTP
		Pattern pattern = Pattern.compile("\n");
		Matcher matcher = pattern.matcher(logStr);    	
		logStr = matcher.replaceAll("");

		MRLog.getInstance(MRLog.CONSOLE).addInfo("AuditEventLogger: LogEvent for userJID:="+userJID+" type:="+auditEvent.type +" name:="+auditEvent.name,MRLog.INFO,MRLog.NOTIFY);

		FileUtils.createDirIfNotExist(SessionServiceConst.AUDIT_EVENT_DIR);
		FileUtils.createFile(SessionServiceConst.AUDIT_EVENT_LOG_FILE);    	    
		FileUtils.appendToFile(SessionServiceConst.AUDIT_EVENT_LOG_FILE, logStr);
	}
	
	private String getGroupsString(Element element,String xpath)  throws MRException,Exception {
		if(element ==null) {
			return "";
		}
		List<String> groupsValues = MRXMLUtils.getValuesXpath(element, xpath);		
		return StringUtils.join(groupsValues, AuditEventLogger.SEPERATOR);
	}

	private String generateLogString(String date, String time, String user,String resource, AuditEvent auditEvent,
			String userGroups,String resourceGroups,String targetGroups)  throws MRException,Exception {
		String descriptionXML ="";
		if(auditEvent.descriptionXML != null)
			descriptionXML = MRXMLUtils.elementToString(auditEvent.descriptionXML);

		return  "<Log type='" + auditEvent.type + "' name='" + auditEvent.name + "' date='" + date +
		"' time='" + time + "' level='" + auditEvent.level + "' user='" + user + "'" +
		" resource='"+resource+"' userGroups='"+userGroups+"' resourceGroups='"+resourceGroups+"' target='"+auditEvent.target+"'" +
				" targetGroups='" +targetGroups+"' status='"+auditEvent.status+"'>" 
		+ " <Description text=\""+auditEvent.descriptionText+"\">" +  descriptionXML + "</Description></Log>";
	}

	private String generateLogStringFromDoc(String date, String time,String user, String resource, AuditEvent auditEvent,
			String userGroups,String resourceGroups,String targetGroups) throws MRException,Exception {
		//as document more readability
		Document auditEventLog = MRXMLUtils.loadXMLResource(AUDIT_EVENT_LOG_XML, MRLog.getInstance(MRLog.CONSOLE));
		Element logEL = auditEventLog.getRootElement();
		MRXMLUtils.setAttributeValue(logEL, DATE, date);
		MRXMLUtils.setAttributeValue(logEL, TIME, time);
		MRXMLUtils.setAttributeValue(logEL, TYPE, auditEvent.type);
		MRXMLUtils.setAttributeValue(logEL, NAME, auditEvent.name);
		MRXMLUtils.setAttributeValue(logEL, LEVEL, auditEvent.level);
		MRXMLUtils.setAttributeValue(logEL, USER, user);
		MRXMLUtils.setAttributeValue(logEL, RESOURCE, resource);
    	MRXMLUtils.setAttributeValue(logEL, USERGROUPS, userGroups);
		MRXMLUtils.setAttributeValue(logEL, RESOURCEGROUPS, resourceGroups);
		MRXMLUtils.setAttributeValue(logEL, TARGET, auditEvent.target);
		MRXMLUtils.setAttributeValue(logEL, TARGETGROUPS, targetGroups);
		MRXMLUtils.setAttributeValue(logEL, STATUS, auditEvent.status);
				
		MRXMLUtils.setAttributeValue(MRXMLUtils.getElementXpath(logEL, "./"+DESCRIPTION), DESCRIPTIONTEXT, auditEvent.descriptionText);
		if(auditEvent.descriptionXML != null)
			MRXMLUtils.addElement(MRXMLUtils.getElementXpath(logEL, "./"+DESCRIPTION),(Element)auditEvent.descriptionXML.clone());

		return  MRXMLUtils.documentToStringOmitDeclaration(auditEventLog);
	}
}

