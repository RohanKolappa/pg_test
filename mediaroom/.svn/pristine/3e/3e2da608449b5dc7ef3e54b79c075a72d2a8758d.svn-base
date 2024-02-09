package com.ipvs.common;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class AgentInitUtils {
	
	public static final int AGENT_STATE_PENDING = 1;
	public static final int AGENT_STATE_WAITING_FOR_RESPONSE_FROM_APPSERVER = 2;
	public static final int AGENT_STATE_ERROR = MRRequest.STATE_EXCEPTION;
	public static final int AGENT_STATE_DONE = MRRequest.STATE_DONE;
	

	public static Document loadAgentInitTasksXML(MRLog log) throws MRException,Exception {
	    return MRXMLUtils.loadXMLResource(MRConst.AGENT_INIT_TASKS_XML_PATH, log);
	}

	public static Element createAgentTask(String name,int  state,String requestName,String requestToken) throws MRException,Exception {
		Element taskEl = MRXMLUtils.newElement(MRConst.TASK_EL_NAME);
		MRXMLUtils.addAttribute(taskEl, MRConst.TASK_NAME_ATTRIBUTE, name);
		MRXMLUtils.addAttribute(taskEl, MRConst.TASK_STATE_ATTRIBUTE, ""+state);
		MRXMLUtils.addAttribute(taskEl, MRConst.TASK_REQUEST_NAME_ATTRIBUTE, requestName);
		MRXMLUtils.addAttribute(taskEl, MRConst.TASK_REQUEST_TOKEN_ATTRIBUTE, requestToken);	
		return taskEl;
	}

	public static void addAgentTask(Document agentInitTaskDoc, String name,int state,String requestName,String requestToken) throws MRException,Exception {
		if(agentInitTaskDoc==null) {
			return;
		}
		
		Element taskEl = createAgentTask(name, state, requestName, requestToken);
		MRXMLUtils.addElement(agentInitTaskDoc.getRootElement(),taskEl);
		MRLog.getInstance(MRLog.CONSOLE).addInfo("AgentInitUtils: Add Agent Task. TaskName:="+name +" requestName:="+requestName+ " requestToken :="+requestToken,
				MRLog.INFO,MRLog.NOTIFY);
	}	
	
	public static void updateAgentStateTask(Document agentInitTaskDoc, String name,int state) throws MRException,Exception {
		
		if(agentInitTaskDoc == null) {
			return;
		}
		
		
		Element taskEl =MRXMLUtils.getElementXpath(agentInitTaskDoc, "//Task[@name='"+name+"']");		
		updateAgentTaskState(taskEl, state);
	}
	
	public static synchronized void updateAgentTaskState(Element taskEl,int state) throws MRException,Exception {
		//if agent task is done no need to update the state
		if(MRXMLUtils.getAttributeValue(taskEl, MRConst.TASK_STATE_ATTRIBUTE,"").equals(AgentInitUtils.AGENT_STATE_DONE)){
			return;
		}
		
		MRXMLUtils.setAttributeValue(taskEl, MRConst.TASK_STATE_ATTRIBUTE, ""+state);
		MRLog.getInstance(MRLog.CONSOLE).addInfo("AgentInitUtils: updateAgentStateTask TaskName:="+MRXMLUtils.getAttributeValue(taskEl, MRConst.TASK_NAME_ATTRIBUTE) 
				+" requestName:="+MRXMLUtils.getAttributeValue(taskEl, MRConst.TASK_REQUEST_NAME_ATTRIBUTE)
				+ " requestToken :="+MRXMLUtils.getAttributeValue(taskEl, MRConst.TASK_REQUEST_TOKEN_ATTRIBUTE)
				+" State:="+state,MRLog.INFO,MRLog.NOTIFY);
	}

	public static String getRequestTokenForGivenTaskName(Document agentInitTaskDoc, String taskName)  throws MRException,Exception {
		
		if(agentInitTaskDoc == null) {
			return Utils.getUUID();
		}
		
		Element taskEl =MRXMLUtils.getElementXpath(agentInitTaskDoc, "//Task[@name='"+taskName+"'");
		return MRXMLUtils.getAttributeValue(taskEl, MRConst.TASK_REQUEST_TOKEN_ATTRIBUTE,Utils.getUUID());
	}
	
	
	public static void updateAgentStateForGivenRequestToken(Document agentInitTaskDoc, String requestToken,int state) throws MRException,Exception {
		
		if(agentInitTaskDoc == null) {
			return;
		}
		
		
		Element taskEl =MRXMLUtils.getElementXpath(agentInitTaskDoc, "//Task[@requestToken='"+requestToken+"']");
		if(taskEl == null) {
			return;
		}
		updateAgentTaskState(taskEl, state);
	}

}
