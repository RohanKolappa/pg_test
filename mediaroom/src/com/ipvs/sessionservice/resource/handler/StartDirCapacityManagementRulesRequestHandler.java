package com.ipvs.sessionservice.resource.handler;

import java.net.URLDecoder;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AddOffloadRequestHandler;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class StartDirCapacityManagementRulesRequestHandler extends FileBaseRequestHandler implements MRRequestHandlerI{

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
				authority);
		log.addTraceEnd(request);

		return auth;
	}

	private static final String IDEL = "Idle";
	private static final String RUNNING_THRESHOLD_RULE ="RunningThresholdRule";
	private static final String RUNNING_ILM_RULE ="RunningILMRule";
	private static final int FILE_COUNT = 5;
	//private static final String ERROR = "Error";

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		RequestProcessor requestProcessor = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.DIR);
		String dirNID  = requestProcessor.getNID();		

		//TBD: Add code to check configuration exist for the directory or not.
		Document dirDoc  = MRXMLDBUtils.readElement(request, log, db, dirNID);
		Element capacityManagementEl = MRXMLUtils.getElementXpath(dirDoc, "//"+MediaStoreServiceConst.CAPACITY_MANAGEMENT);
		Element thresholdEl  = MRXMLUtils.getElement(capacityManagementEl, MediaStoreServiceConst.THRESHOLD_RULE);
		Element ilmRuleEl = MRXMLUtils.getElement(capacityManagementEl, MediaStoreServiceConst.ILM_RULE);

		String thresholdRuleEnabled = MRXMLUtils.getAttributeValue(thresholdEl, MediaStoreServiceConst.ENABLE_ATTRIBUTE,"");
		String ilmRuleEnabled = MRXMLUtils.getAttributeValue(ilmRuleEl, MediaStoreServiceConst.ENABLE_ATTRIBUTE,"");
		if((thresholdRuleEnabled.isEmpty() || thresholdRuleEnabled.equals(""+Boolean.FALSE)) && (ilmRuleEnabled.isEmpty() || ilmRuleEnabled.equals(""+Boolean.FALSE))) {
			log.addInfo("StartDirCapacityManagementRulesRequestHandler: Rules not enabled. thresholdRuleEnabled:="+thresholdRuleEnabled+" ilmRuleEnabled:="+ilmRuleEnabled+" dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
			return request.createResponseUsingRequestRootName(null);
		}

		String nextState= "";
		String currentState = MRXMLUtils.getAttributeValue(capacityManagementEl, SessionServiceConst.STATE,IDEL);
		int lastRunState = MRRequest.STATE_DONE;

		try {
			boolean isStartingProcess = false;
			if(currentState.isEmpty() || currentState.equals(IDEL)){
				isStartingProcess = true;
			}
			
			log.addInfo("StartDirCapacityManagementRulesRequestHandler: CurrentState Of Schedule is state:="+currentState,MRLog.INFO,MRLog.NOTIFY);
			
			if((currentState.isEmpty() || currentState.equals(IDEL) || currentState.equals(RUNNING_ILM_RULE)) && ilmRuleEnabled.equals(SessionServiceConst.TRUE)) {
				nextState = runILMRule(dirNID,dirDoc,requestProcessor,capacityManagementEl,ilmRuleEl,thresholdEl,isStartingProcess,request,xmpp,log,db,resourceFactory,serviceAgentFactory);
			} else if(thresholdRuleEnabled.equals(SessionServiceConst.TRUE)){
				nextState = runThresholdRule(dirNID,dirDoc,requestProcessor,capacityManagementEl,thresholdEl,isStartingProcess,request,xmpp,log,db,resourceFactory,serviceAgentFactory);
			}

		} catch (MRException e) {	
			lastRunState = e.getCode();
			nextState=IDEL;
			e.printStackTrace();
		} catch (Exception e) {			
			lastRunState = MRRequest.STATE_EXCEPTION;
			nextState=IDEL;
			e.printStackTrace();
		}

		//update dir capacity management rule state
		//if the current state is idel we have to update the last run date and time 
		//if the current state ! = existing state then also we have to update the last run state
		//if the last run state is error also we have to update the state		
		
		if(currentState.equals(IDEL) || !currentState.equals(nextState) || lastRunState!=MRRequest.STATE_DONE){
			request.setSuperUser(true);
			updateDirCapacityManagementRulesState(dirNID,dirDoc, nextState, lastRunState,request,xmpp,db,log,serviceAgentFactory,resourceFactory);
			request.setSuperUser(false);
		}

		return request.createResponseUsingRequestRootName(null);
	}

	private void updateDirCapacityManagementRulesState(String dirNID,Document dirDoc, String nextState, int lastRunState, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory, 
			MRResourceFactoryI resourceFactory) throws Exception {
		Element capacityManagementEl = MRXMLUtils.getElementXpath(dirDoc, "//"+MediaStoreServiceConst.CAPACITY_MANAGEMENT);
		MRXMLUtils.setAttributeValue(capacityManagementEl, SessionServiceConst.STATE, nextState);
		//if the state is idle update last run state
		if(nextState.equals(IDEL)){
			SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yy HH:mm:ss"); 
			String currentDateTime = simpleDateFormat.format(new Date());		
			int index = currentDateTime.indexOf(" ");
			String currentDate = currentDateTime.substring(0, index);
			String currentTime = currentDateTime.substring(index + 1);

			MRXMLUtils.setAttributeValue(capacityManagementEl, MediaStoreServiceConst.LAST_RUN_STATE, ""+lastRunState);
			MRXMLUtils.setAttributeValue(capacityManagementEl, MediaStoreServiceConst.LAST_RUN_DATE, ""+currentDate);
			MRXMLUtils.setAttributeValue(capacityManagementEl, MediaStoreServiceConst.LAST_RUN_TIME, ""+currentTime);
		}
		
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: updateDirCapacityManagementRulesState:="+nextState + " dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		UpdateObjectUtils.updateElement(request, xmpp, log, db, AMConst.MEDIA_STREAM_DIR_RESOURCE, dirNID, dirDoc, "//"+MediaStoreServiceConst.CAPACITY_MANAGEMENT);
	}

	private String runILMRule(String dirNID, Document dirDoc, RequestProcessor requestProcessor, Element capacityManagementEl,
			Element ilmRuleEl,Element thresholdEl, boolean isStartingProcess, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)  throws Exception  {
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: Running ILMRule dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		String nextState = runRule(dirNID,dirDoc, requestProcessor, ilmRuleEl, isStartingProcess, request,xmpp,log,db,resourceFactory,serviceAgentFactory);
		
		if(nextState.equals(RUNNING_THRESHOLD_RULE) && MRXMLUtils.getAttributeValue(thresholdEl, SessionServiceResourceConst.ENABLE_ATTRIBUTE,"").equals(SessionServiceConst.TRUE)){		
			return runThresholdRule(dirNID, dirDoc, requestProcessor, capacityManagementEl, thresholdEl, true, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}	else if(nextState.equals(RUNNING_THRESHOLD_RULE) && !MRXMLUtils.getAttributeValue(thresholdEl, SessionServiceResourceConst.ENABLE_ATTRIBUTE,"").equals(SessionServiceConst.TRUE)){	
				log.addInfo("StartDirCapacityManagementRulesRequestHandler: No threshold Rule Enabled. Set State as Idel dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
				nextState = IDEL;
		}
		return nextState;
	}


	private String runThresholdRule(String dirNID, Document dirDoc, RequestProcessor requestProcessor,
			Element capacityManagementEl, Element thresholdEl, boolean isStartingProcess, MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: Running ThresholdRule dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		return runRule(dirNID, dirDoc, requestProcessor, thresholdEl,isStartingProcess, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}

	private String runRule(String dirNID, Document dirDoc, RequestProcessor requestProcessor, Element ruleEl, boolean isStartingProcess, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory)  throws Exception {

		String ruleName = ruleEl.getName();
		String ruleType = MRXMLUtils.getAttributeValue(ruleEl, SessionServiceConst.TYPE_ATTRIBUTE,"");
		//if the rule type is offload make sure no pending requests in export queue
		if(ruleType.equals(MediaStoreServiceConst.OFFLOAD_RULE)) {				
			if(pendingAutoExportFilesNotInError(dirNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory)){
				return ruleName.equals(MediaStoreServiceConst.THRESHOLD_RULE)?RUNNING_THRESHOLD_RULE:RUNNING_ILM_RULE; 
			}			
		}
		
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: RuleType:="+ruleType+" ruleName : "+ruleName+" dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);		

		List<?> listOfFiles = getListOfFilesToBeOffload(dirNID, ruleName, ruleEl, ruleType, isStartingProcess, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	
		if(listOfFiles.isEmpty() && ruleName.equals(MediaStoreServiceConst.THRESHOLD_RULE)){
	    	log.addInfo("StartDirCapacityManagementRulesRequestHandler: nextState:="+IDEL+" dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		    return IDEL;
		} else if(listOfFiles.isEmpty() && ruleName.equals(MediaStoreServiceConst.ILM_RULE) ){
			log.addInfo("StartDirCapacityManagementRulesRequestHandler: nextState:="+RUNNING_THRESHOLD_RULE+" dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
			return RUNNING_THRESHOLD_RULE;				
		}
		
		if(ruleType.equals(MediaStoreServiceConst.OFFLOAD_RULE)){			
			String xmlFilePath = "/com/ipvs/assetadminservice/xml/AddOffloadRequest.xml";
	        Document addOffloadRequestDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);	
	        Element fileEL = MRXMLUtils.getElementXpath(addOffloadRequestDocument, "//"+SessionServiceConst.FILE);
	        fileEL.detach();
	        Element requestEl = MRXMLUtils.getElementXpath(addOffloadRequestDocument, "//AddOffload");
	        for(Object object:listOfFiles) {
	        	Element element = (Element) object;
	        	MRXMLUtils.setAttributeValue(fileEL, SessionServiceConst.FILENID, MRXMLUtils.getAttributeValue(element, SessionServiceConst.NID,""));
	        	MRXMLUtils.addElement(requestEl, (Element)fileEL.clone());
	        }	        
	        MRXMLUtils.setAttributeValue(requestEl, MediaStoreServiceConst.NAS_URL_ATTRIBUTE, MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.NAS_DRIVE_URL_ATTRIBUTE,""));
	        MRXMLUtils.setAttributeValue(requestEl, MediaStoreServiceConst.FORMAT_ATTRIBUTE, MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.FORMAT_ATTRIBUTE,""));
	        MRRequest offloadRequest = MRRequest.createRequest(request.getAppServerJID(), addOffloadRequestDocument);	        
	        AddOffloadRequestHandler offloadRequestHandler = new AddOffloadRequestHandler();
	        offloadRequestHandler.getResponse(offloadRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);	        
		} else {

			String requestName = MRRequest.getRequestName(DeleteFileRequestHandler.class.getName());
			Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
			Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log);
			Document deleteFileReqDoc = generateRequestDocument(requestName, getFileMapping(listOfFiles), dirNID, mediaStorePortElement, "", "", "", "", null, request.getNID(),log);
			//add purge files flag. This will delete the files from disk with out moving to trash folder
			Element deleteRequestEl = MRXMLUtils.getElementXpath(deleteFileReqDoc, "//"+MediaStoreServiceConst.DELETE_FILE_REQUEST);
			MRXMLUtils.setAttributeValue(deleteRequestEl, MediaStoreServiceConst.PURGE_FILES_ATTRIBUTE, MediaStoreServiceConst.TRUE);
			sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, deleteFileReqDoc, mediaStorePortElement, this.getClass().getName());
		}
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: nextState:="+(ruleName.equals(MediaStoreServiceConst.THRESHOLD_RULE)?RUNNING_THRESHOLD_RULE:RUNNING_ILM_RULE)+" dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		return ruleName.equals(MediaStoreServiceConst.THRESHOLD_RULE)?RUNNING_THRESHOLD_RULE:RUNNING_ILM_RULE;
	}

	private List<?> getListOfFilesToBeOffload(String dirNID,String ruleName,Element ruleEl,String ruleType,  boolean isStartingProcess, MRRequest request, XMPPI xmpp, MRLog log,MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		String queryFileName = SessionServiceConst.XQUERYPACKAGE + ruleName + SessionServiceConst.XQUERYXML;
		String query = FileUtils.readFileResource(queryFileName, request.getClass());

		if(ruleName.equals(MediaStoreServiceConst.THRESHOLD_RULE)) {			
			//if it is start of the process then use higher limit else check with lower limit
			if(isStartingProcess) {
				query = query.replace(SessionServiceResourceConst._DISKCAPACITYREACHED_,MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.DISK_CAPACITY_REACHED_ATTRIBUTE,""));
			} else {
				query = query.replace(SessionServiceResourceConst._DISKCAPACITYREACHED_,MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.REDUCE_DISK_CAPACITY_ATTRIBUTE,""));
			}	
			
			log.addInfo("StartDirCapacityManagementRulesRequestHandler: HigherLimit:="+MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.DISK_CAPACITY_REACHED_ATTRIBUTE,"") +" LowerLimit:="+
					MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.REDUCE_DISK_CAPACITY_ATTRIBUTE,"") +" isStartingProcess"+isStartingProcess,MRLog.INFO,MRLog.NOTIFY);
		} else {			
			Date toDate = new Date();
			String olderThan = MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.OLDER_THAN_ATTRIBUTE,"");
			if(olderThan.isEmpty()){
				throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid Input");
			}
			long timestamp = GetObjectUtils.calculateDate(toDate, olderThan, "-");
			query = query.replace(SessionServiceResourceConst._DATETIMESTAMP_,String.valueOf(timestamp));
		}
				
		query = query.replace(SessionServiceResourceConst._DIRNID_,dirNID);
		if(ruleType.equals(MediaStoreServiceConst.DELETE)) {
			query = query.replace(SessionServiceResourceConst._SKIPERRORS_,SessionServiceConst.FALSE);
		} else {			
			query = query.replace(SessionServiceResourceConst._SKIPERRORS_, String.valueOf(!isStartingProcess));
		}
		
		String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
		Document responseDoc  = MRXMLUtils.stringToDocument(responseXML);

		List<?> listOfFiles = MRXMLUtils.getListXpath(responseDoc, "//"+AMConst.MEDIA_STREAM_FILE_RESOURCE);
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: listOfFiles Found for rule:="+ruleName+" dirNID:="+dirNID +" fileList:="+listOfFiles.size(),MRLog.INFO,MRLog.NOTIFY);
		
		if(!listOfFiles.isEmpty() && ruleName.equals(MediaStoreServiceConst.THRESHOLD_RULE)) {
			long lowerThreshouldLimit = Utils.getLongValue(MRXMLUtils.getAttributeValue(ruleEl, MediaStoreServiceConst.REDUCE_DISK_CAPACITY_ATTRIBUTE,""),0);
			long sizeUsed = Utils.getLongValue(MRXMLUtils.getAttributeValue(responseDoc.getRootElement(), SessionServiceConst.SIZE_USED,""),0);
			List<?> modifyList = listOfFiles;
			int filecount = FILE_COUNT;
			if(listOfFiles.size() < filecount)
				filecount = listOfFiles.size();
			
			int newIndex = validateListBasedOnLowerThresholdLimit(lowerThreshouldLimit,(Document)responseDoc.clone(), modifyList,sizeUsed,(filecount), log);
			log.addInfo("StartDirCapacityManagementRulesRequestHandler: Modified Index:="+newIndex,MRLog.INFO,MRLog.NOTIFY);
			return listOfFiles.subList(0, newIndex);
		}
		
		return listOfFiles;
	}

	private int validateListBasedOnLowerThresholdLimit(long lowerThreshouldLimit, Document responseDoc, List<?> listOfFiles, long sizeUsed, int index, MRLog log) throws Exception {

		List<String> values = MRXMLUtils.getValuesXpath(responseDoc, "//Size");
		long totalFileSize = 0;

		for(String strSize : values){
			totalFileSize += Utils.getLongValue(strSize, 0);
		}
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: index:="+index +" totalFileSize:="+totalFileSize+" dirSize:="+sizeUsed +" diff:="+(sizeUsed - totalFileSize) +" lowerLimit:="+lowerThreshouldLimit,MRLog.INFO,MRLog.NOTIFY);
		if((sizeUsed - totalFileSize )  > lowerThreshouldLimit || index == 1){
			return index;
		} else {
			Element fileEl =(Element) listOfFiles.get(index-1);
			MRXMLUtils.removeNodeXpath(responseDoc, "//MediaStreamFileResource[@NID='"+MRXMLUtils.getAttributeValue(fileEl, SessionServiceConst.NID,"")+"']");	
			listOfFiles.remove(index-1);
			return validateListBasedOnLowerThresholdLimit(lowerThreshouldLimit, responseDoc, listOfFiles, sizeUsed, index-1, log);
		}
	}

	private boolean pendingAutoExportFilesNotInError(String dirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		String query = " let $dirNID := '"+dirNID+"'" +"\n" +
		"let $pendingCount := count(for $offloadEntry in collection('ipvs.default.exportimportfilelist')/*" +
		"								[.//Internal/@dirNID = $dirNID and   @type='Offload'  and (@state_description!='Error') and (@state_description!='Completed')]" +
		"  							return $offloadEntry)\n" +
		"let $errorCount := count(for $offloadEntry in collection('/db/ipvs.default.exportimportfilelist')/*" +
		"							[.//Internal/@dirNID = $dirNID and @type='Offload' and @state_description='Error']" +
		"  						  return $offloadEntry)\n" +
		"return <Dir NID='{$dirNID}'  pending='{$pendingCount }' error='{$errorCount}'/>";		
	
		String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
		if(responseXML ==null || responseXML.isEmpty()) {
			return true;
		}

		Document dirDocument  = MRXMLUtils.stringToDocument(responseXML);
		int pendingCount = Utils.getIntValue(MRXMLUtils.getAttributeValue(dirDocument.getRootElement(), SessionServiceResourceConst.PENDING_ATTRIBUTE,""), 0); 
		int errorCount = Utils.getIntValue(MRXMLUtils.getAttributeValue(dirDocument.getRootElement(), SessionServiceResourceConst.ERROR_ATTRIBUTE,""), 0);

		log.addInfo("StartDirCapacityManagementRulesRequestHandler: Error Files in the database."+errorCount,MRLog.INFO,MRLog.NOTIFY);
		//if error count is more than 50 for the directory ignore the process update the dir state
		if(errorCount >= 50) {
			throw new MRException(SessionServiceExceptionCodes.EXCEEDED_ERROR_LIMIT, "Exceeded number of offload errors. Please check the directory list");
		}
		log.addInfo("StartDirCapacityManagementRulesRequestHandler: Pending Files in the database."+pendingCount,MRLog.INFO,MRLog.NOTIFY);
		//if pending list is pending in queue then return true
		if(pendingCount > 0) {
			return true;
		}

		return false;
	}

	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {	

		Document agentResponseDoc = request.getResult();	
		String clientData = MRXMLUtils.getValueXpath(agentResponseDoc, "//"+MRConst.CLIENT_DATA, "");
		Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
		//get RequestName
		String deleteFileRequestName = MRRequest.getRequestName(DeleteFileRequestHandler.class.getName());
	
		//create user request
		MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);	
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		RequestProcessor requestProcessor = new RequestProcessor(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.DIR);
		String dirNID  = requestProcessor.getNID();		
		
		Document dirDoc  = MRXMLDBUtils.readElement(request, log, db, dirNID);
		log.addInfo("StartDirCapacityManagementRulesRequestHandler : handleAgentResponse. state:="+state +" dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			int lastRunState = Utils.getIntValue(MRXMLUtils.getValueXpath(agentResponseDoc, "//Code", ""), SessionServiceExceptionCodes.AGENT_RESPONSE_FAILED);
			updateDirCapacityManagementRulesState(dirNID, dirDoc, IDEL, lastRunState, userRequest, xmpp, db, log, serviceAgentFactory, resourceFactory);
			return;
		}
		List<String> mediaStreamfileList = null;
		if (deleteFileRequestName.equals(request.getRequestName())) {
			mediaStreamfileList = MRXMLUtils.getValuesXpath(agentResponseDoc, "//File/@NID");
		} else {
			mediaStreamfileList = MRXMLUtils.getValuesXpath(agentResponseDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID);
		}

		DeleteFileRequestHandler requestHandler = new DeleteFileRequestHandler();
		for (String fileNID : mediaStreamfileList) {
			
			Document responseDoc= requestHandler.deleteFileAfterAgentResponse(xmpp, log, db, resourceFactory, serviceAgentFactory, userRequest, agentResponseDoc, fileNID);
			// Add delete event to audit log
			Element fileEl= MRXMLUtils.getElementXpath(responseDoc, "File");
			String fileTitle = MRXMLUtils.getAttributeValueXpath(fileEl, "//title", "");
			String descText = MRXMLUtils.getValueXpath(fileEl, "//type", "");
			
			
			AuditEvent auditEvent = new AuditEvent.Builder(AuditEventLogger.MEDIA_EVENT, MediaRoomServiceConst.DELETE, request.getUserJID(), AuditEventLogger.LEVEL_INFO)
					.status(AuditEventLogger.STATUS_SUCCESS).descriptionText(descText).target(fileTitle).build();
			AuditEventLogger.getInstance().addAuditEventToQueue(auditEvent);
		}
		userRequest.setCache(new MRRequestCache());
		userRequest.getCache().put(dirNID, dirDoc);	
		//postpone calling get response till we received  purge file porcess complete notfication from DMS
		
		//getResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);		
	}
}
