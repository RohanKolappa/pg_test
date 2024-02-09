package com.ipvs.sessionservice.resource.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Node;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;

public class SetDirStorageConfigRequestHandler extends FileBaseRequestHandler {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		RequestProcessor requestProcessor = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.DIR);
		String dirNID  = requestProcessor.getNID();
		Element requestEl = requestProcessor.getRequestElement();
		String whereMode = MRXMLUtils.getAttributeValue(requestEl, SessionServiceResourceConst.WHERE_MODE,"");
		if(!whereMode.equals(SessionServiceResourceConst.DIR_SPANNING) && !whereMode.equals(SessionServiceResourceConst.CAPACITY_MANAGEMENT)){ 
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid Input");
		}
		
		Document dirDoc = MRXMLDBUtils.readElement(request, log, db, dirNID);
			
		if(whereMode.equals(SessionServiceResourceConst.DIR_SPANNING)){
			updateDirSpanningRule(dirNID,dirDoc,requestEl,request, xmpp, log, db, resourceFactory, serviceAgentFactory);				
		} else if(whereMode.equals(SessionServiceResourceConst.CAPACITY_MANAGEMENT)){
			updateCapacityManagement(dirNID,dirDoc,requestEl,request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}			
		return GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestProcessor, 
				SessionServiceConst.SELECTFORMAT_EXTENDED, SessionServiceConst.GET_DIR_REQUEST_XML, GetDirRequestHandler.class.getName());
	}

	private void updateCapacityManagement(String dirNID, Document dirDoc, Element requestEl, MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Element requestCapacityManagementEl = MRXMLUtils.getElement(requestEl, SessionServiceResourceConst.CAPACITY_MANAGEMENT);
		String schedule  = MRXMLUtils.getAttributeValue(requestCapacityManagementEl, SessionServiceResourceConst.SCHEDULE_ATTRIBUTE_NAME,"");
		Element capacityManagementEl =  MRXMLUtils.getElementXpath(dirDoc,"//"+ SessionServiceResourceConst.CAPACITY_MANAGEMENT);
		MRXMLUtils.setAttributeValue(capacityManagementEl, SessionServiceResourceConst.SCHEDULE_ATTRIBUTE_NAME, schedule);
		MRXMLUtils.replaceContent(capacityManagementEl, requestCapacityManagementEl);
		
		Element dirSpanningEl =  MRXMLUtils.getElement(dirDoc,"//"+ SessionServiceResourceConst.DIR_SPANNING);
		MRXMLUtils.setAttributeValue(dirSpanningEl, SessionServiceResourceConst.DISK_FULL_THRESHOLD, ""+MediaStoreServiceConst.DISK_QUOTA_EXCEED_LIMIT);
		
		updateInDataBase(dirNID, dirDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceResourceConst.CAPACITY_MANAGEMENT);
		
		//Start scheduler.	
		updateTaskInScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirDoc, dirNID);
	}

	private void updateDirSpanningRule(String dirNID,Document dirDoc, Element requestEl, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
		
		Element requestDirSpanningEl = MRXMLUtils.getElement(requestEl, SessionServiceResourceConst.DIR_SPANNING);
		String threshold = MRXMLUtils.getAttributeValue(requestDirSpanningEl, SessionServiceResourceConst.DISK_FULL_THRESHOLD,"");
		Element dirSpanningEl =  MRXMLUtils.getElementXpath(dirDoc,"//"+ MediaStoreServiceConst.MEDIA_DIR_SPANNING);	
		MRXMLUtils.setAttributeValue(dirSpanningEl, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, threshold);	
		List<?> portList = MRXMLUtils.getListXpath(requestDirSpanningEl, ".//"+SessionServiceResourceConst.PORT_ELEMENT);
		for(Object object:portList) {
			Element portEl =(Element) object;
			MRXMLUtils.setAttributeValue(MRXMLUtils.getElementXpath(dirSpanningEl, 
					".//"+MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME+"[.='"+MRXMLUtils.getAttributeValue(portEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE)+"']"),
					MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME, MRXMLUtils.getAttributeValue(portEl,MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME,""+MediaStoreServiceConst.DEFAULT_PRIORITY));
		}
		
		//reset Capacity management values
		Element capacityManagementEl =  MRXMLUtils.getElementXpath(dirDoc,"//"+ SessionServiceResourceConst.CAPACITY_MANAGEMENT);
		MRXMLUtils.setAttributeValue(capacityManagementEl, SessionServiceResourceConst.SCHEDULE_ATTRIBUTE_NAME, "");
		resetValues(MRXMLUtils.getElement(capacityManagementEl, MediaStoreServiceConst.THRESHOLD_RULE));
		resetValues(MRXMLUtils.getElement(capacityManagementEl, MediaStoreServiceConst.ILM_RULE));
		
		updateInDataBase(dirNID, dirDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceResourceConst.DIR_SPANNING);
		
		
		String state = MRXMLUtils.getValueXpath(dirDoc, "//"+AMConst.STATEFLAG, "");
		if(!state.equals(MediaStoreServiceConst.READY)){
			return;
		}
		
		//send directory updates
		Document dirUpdateRequest  = createRequestDocument("UpdateDirThresholdRequest", dirNID, "", log);
		Element dirUpdateElRequest = MRXMLUtils.getElementXpath(dirUpdateRequest, "//UpdateDirThresholdRequest");
		MRXMLUtils.setAttributeValue(dirUpdateElRequest, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD, threshold);
		MRRequest userRequest = MRRequest.createRequest(request.getAppServerJID());
		List<?> mediaStorePortList = MRXMLUtils.getListXpath(dirDoc, ".//"+MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME);
		for(Object object:mediaStorePortList) {
			Element element = (Element) object;
			String presence  = MRXMLUtils.getAttributeValue(element, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME,"");
			if(presence.equals(AMConst.AVAILABLE)) {
				MRXMLUtils.setAttributeValue(dirUpdateElRequest, "byDirID", MRXMLUtils.getAttributeValue(element, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME,""));				
				sendMediaStoreRequestToDMS(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, (Document) dirUpdateRequest.clone(), element, null);
			}
		}
		
		//Stop Scheduler if running
		log.addInfo("SetDirStorageConfigRequestHandler::Deleting Dir task from the scheduler", MRLog.OK, MRLog.NOTIFY);		
		AppServerAgentUtils.getScheduleManager(serviceAgentFactory).deleteTaskFromScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirNID);
	}
	
	private void resetValues(Element element ){
		List<?> nodeList = element.attributes();
		
		for(Object object:nodeList) {
			Node node = (Node) object;
			node.setText("");
		}
	}
	
	private  void updateInDataBase(String dirNID,Document dirDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String mode) throws Exception {
		MRXMLUtils.setAttributeValueXPath(dirDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_STORAGE_CONFIG, SessionServiceResourceConst.MODE_ATTRIBUTE, mode);
		UpdateObjectUtils.updateElement(request, xmpp, log, db, AMConst.MEDIA_STREAM_DIR_RESOURCE, dirNID, dirDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_STORAGE_CONFIG);
	}
	
	public void updateTaskInScheduler(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document reportObjectXMLDoc, String NID) throws Exception
	{			
		log.addInfo("SetDirStorageConfigHandler::Deleting the existing Dir task if any", MRLog.OK, MRLog.NOTIFY);
		AppServerAgentUtils.getScheduleManager(serviceAgentFactory).deleteTaskFromScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID);
		
		log.addInfo("SetDirStorageConfigHandler::Adding a new Dir task to the scheduler queue", MRLog.OK, MRLog.NOTIFY);
		addDirTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, reportObjectXMLDoc, NID);
	}
	
	public static void addDirTaskToScheduler(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document objectXMLDoc, String NID) throws Exception 
	{	
		String schedule = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, "//" + MediaStoreServiceConst.CAPACITY_MANAGEMENT, "schedule", "");
		String enableThresholdRule = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, "//" + MediaStoreServiceConst.THRESHOLD_RULE, MediaStoreServiceConst.ENABLE_ATTRIBUTE, "");
		String enableILMRule = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, "//" + MediaStoreServiceConst.ILM_RULE, MediaStoreServiceConst.ENABLE_ATTRIBUTE, "");
		
		if ((!enableThresholdRule.equals(SessionServiceConst.TRUE) && !enableILMRule.equals(SessionServiceConst.TRUE)) || schedule.isEmpty()) {		
			return;
		}
		
		// Create StartDirCapacityManagementRules requestXML 
		Element parentEl = MRXMLUtils.newElement(SessionServiceConst.START_DIR_CAPACITY_MANAGEMENT_RULES);
        MRXMLUtils.addAttribute(parentEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, NID);
        Document requestDoc = SessionServiceUtils.getRequestXML(SessionServiceConst.START_DIR_CAPACITY_MANAGEMENT_RULES_REQUEST, log, parentEl); 
        
        log.addInfo("SetDirStorageConfigHandler::Schedule Dir Capacity Management Rule", MRLog.OK, MRLog.NOTIFY);
        AppServerAgentUtils.getScheduleManager(serviceAgentFactory).addTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, schedule, 
        		NID, requestDoc);
	}
}
