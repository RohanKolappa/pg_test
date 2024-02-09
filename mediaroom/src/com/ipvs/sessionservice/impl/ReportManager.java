package com.ipvs.sessionservice.impl;

import java.net.URLDecoder;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.impl.TargetObjectLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.sessionservice.resource.handler.GetTagsRequestHandler;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class ReportManager {
	
	private final String USER_POLICY_REPORT_INSTANCE_XML = "/com/ipvs/sessionservice/datamodel/UserPolicyReportInstance.xml";
	private final String GET_TAGS_REQUEST_XML = "/com/ipvs/sessionservice/resource/xml/GetTagsRequest.xml";
	private static final ReportManager theReportManager = new ReportManager();
	
	/* Prevent from creating instance of the class from other classes */
	private ReportManager() {}
	
	public static ReportManager getInstance() 
	{
		return theReportManager;
	}
	
	public void initializeReportScheduler(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String appServerAgentJID, String userJID) throws Exception
	{	
		log.addInfo("ReportManager::Adding report tasks to the scheduler", MRLog.OK, MRLog.NOTIFY);
		
		MRRequest request = MRRequest.createRequest(userJID, appServerAgentJID);		
		String[] whereXPath = { "//EnableExport='true'" };
		Element dbResponseEl = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, "", SessionServiceConst.REPORT_OBJECT_TYPE, "", "", whereXPath, null, "", "", null, 0, -1);
		List<?> reportElList = MRXMLUtils.getListXpath(dbResponseEl, "//" + SessionServiceConst.REPORT_OBJECT_TYPE);
		
		for (Object obj : reportElList)
		{
			Document reportObjectXMLDoc = MRXMLUtils.elementToDocument((Element) obj);
			String whereNID = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.REPORT_OBJECT_TYPE, SessionServiceConst.NID, "");
			try {			
				addReportTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, reportObjectXMLDoc, whereNID);
			}catch (Exception e) {
				log.addInfo("ReportManager::Adding report tasks to the scheduler..Failed! : " + e.toString());
				e.printStackTrace();
			}
		}
				
	}
	
	public void updateTaskInScheduler(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document reportObjectXMLDoc, String NID) throws Exception
	{			
		log.addInfo("ReportManager::Deleting the existing Report task if any", MRLog.OK, MRLog.NOTIFY);
		AppServerAgentUtils.getScheduleManager(serviceAgentFactory).deleteTaskFromScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID);
		
		log.addInfo("ReportManager::Adding a new report task to the scheduler queue", MRLog.OK, MRLog.NOTIFY);
		addReportTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, reportObjectXMLDoc, NID);
	}
	
	public void addReportTaskToScheduler(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document reportObjectXMLDoc, String NID) throws Exception 
	{	
		String enableExport = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.ENABLE_EXPORT_EL_NAME, "");
		String exportSchedule = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.EXPORT_SCHEDULE_EL_NAME, "");
		String exportURL = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.EXPORT_URL_EL_NAME, "");
		
		if (!enableExport.equalsIgnoreCase(SessionServiceConst.TRUE) || exportSchedule.isEmpty() || exportURL.isEmpty()) {		
			return;
		}
		
		// Create ExportReport requestXML 
		Element parentEl = MRXMLUtils.newElement(SessionServiceConst.EXPORT_REPORT_EL_NAME);
        MRXMLUtils.addAttribute(parentEl, SessionServiceConst.WHERE_NID, NID);
        Document requestDoc = SessionServiceUtils.getRequestXML(SessionServiceConst.EXPORT_REPORT_REQUEST, log, parentEl); 
        
        log.addInfo("ReportManager::Schedule report FTP", MRLog.OK, MRLog.NOTIFY);
        AppServerAgentUtils.getScheduleManager(serviceAgentFactory).addTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, exportSchedule, 
        		NID, requestDoc);
	}
	
	public synchronized String executeFTPCmd(String outputFile, String exportURL, String destFileName, String workAreaDir) throws Exception
    {
    	String state = "";
    	
    	String[] systemCmd = {"sh", "/etc/scripts/ftp_upload.sh", outputFile, exportURL, destFileName, workAreaDir};
		if(MediaRoomServiceUtils.executeSystemCommand(systemCmd) != 0) {
			state = SessionServiceConst.FAILURE;
		} else {
			state = SessionServiceConst.SUCCESS;
		}
		
		return state;
    }
	
	public Document runReport(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject, String action) throws Exception
	{					
		Document reportObjectXMLDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestObject, "", "");
		String NID = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + requestObject.getObjectType(), SessionServiceConst.NID, "");
		
		Document requestDoc = request.getMessage().getDocument();		
		MRXMLUtils.setAttributeValueXPath(requestDoc, "//" + SessionServiceConst.DOWNLOAD_REPORT_EL_NAME, SessionServiceConst.WHERE_NID, NID);
				
		log.addInfo("ReportManager::Starting runReport for report with NID = " + NID, MRLog.OK, MRLog.NOTIFY);			
		
		// Get the values from the report object
		String reportName = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.NAME_EL_NAME, "");			
		String type = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.TYPE_EL_NAME, "");			
		String outputFileExtension = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + type + SessionServiceConst.REPORT_CONFIG_EL_NAME, SessionServiceConst.SELECTFORMAT, "CSV");
		String formatXSL = URLDecoder.decode(MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.SELECT + SessionServiceConst.FORMAT_XSL_EL_NAME, ""), "UTF-8");
		String enableExport = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.ENABLE_EXPORT_EL_NAME, "");			
		String exportURL = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.EXPORT_URL_EL_NAME, "");						
		String whereTarget = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.POLICY + SessionServiceConst.REPORT_CONFIG_EL_NAME, SessionServiceConst.WHERE_TARGET_ATTR_NAME, "");
		String whereTagExpr = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.POLICY + SessionServiceConst.REPORT_CONFIG_EL_NAME, SessionServiceConst.WHERE_TAG_EXPR, "");
		String whereTextSubStr = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.POLICY + SessionServiceConst.REPORT_CONFIG_EL_NAME, SessionServiceConst.WHERE_TEXT_SUB_STR, "");
		String lastExportedDate = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.LAST_EXPORTED_EL_NAME, SessionServiceConst.DATE, "");		
		String lastExportedTime = MRXMLUtils.getAttributeValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.LAST_EXPORTED_EL_NAME, SessionServiceConst.TIME, "");
		
		// Get current date and time				
		SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yy HH:mm:ss"); 
		String currentDateTime = simpleDateFormat.format(new Date());		
		int index = currentDateTime.indexOf(" ");
		String currentDate = currentDateTime.substring(0, index);
		String currentTime = currentDateTime.substring(index + 1);
		
		// Get time in seconds based on the date and time got from the db
		String prevTimestamp = "0";
		if (!lastExportedDate.isEmpty() && !lastExportedTime.isEmpty()) {
			prevTimestamp = String.valueOf(getTimeInSecondsSinceEpoch(simpleDateFormat, lastExportedDate, lastExportedTime));
		}
		
		// Get time in seconds based on the current date and time
		String currTimestamp = String.valueOf(getTimeInSecondsSinceEpoch(simpleDateFormat, currentDate, currentTime));
		
		// Create the work area directory and the necessary files
		
		String jobID = "WORKAREA_" + Utils.getUUID();
		String liveDirPath = SessionServiceConst.REPORTS_DIR + "/" + jobID;
		
		MRXMLUtils.setAttributeValueXPath(requestDoc, "//" + SessionServiceConst.DOWNLOAD_REPORT_EL_NAME, SessionServiceConst.LIVE_DIR, liveDirPath);
		
		String workAreaDirName = "";
		if (action.equals(SessionServiceConst.DOWNLOAD_REPORT)) {			
			workAreaDirName = SessionServiceConst.DOWNLOAD_REPORT_FOLDER_NAME;
		} else { 			
			workAreaDirName = SessionServiceConst.FTP_REPORT_FOLDER_NAME;
		}
		
		String workAreaDirPath = liveDirPath + "/" + workAreaDirName;
		
		log.addInfo("ReportManager::Create the work area dir if it doesn't exist: workArea = " + workAreaDirPath, MRLog.OK, MRLog.NOTIFY);		
		FileUtils.createDirIfNotExist(workAreaDirPath);		
		
		String reportType = type.toLowerCase();				
		String inputFileName = reportType + SessionServiceConst.LOG_EXT;
		String xslFileName = reportType + SessionServiceConst.XSL_EXT;
		
		// The file name is generated only for FTP
		String outputFileName = "";		
		if (outputFileExtension.equals(SessionServiceConst.XML)) {
			outputFileName = reportType + SessionServiceConst.XML_EXT;
		} else {
			outputFileName = reportType + SessionServiceConst.CSV_EXT;
		}
				
		String destFileName = "";
		if (action.equals(SessionServiceConst.FTP_REPORT))
		{
			int indexOfDot = outputFileName.indexOf(".");
			String fileExtension = outputFileName.substring(indexOfDot);
			SimpleDateFormat simpleDateFormatObj = new SimpleDateFormat("yy-MM-dd_HH.mm.ss");
			String timestamp = simpleDateFormatObj.format(new Date());			
			destFileName = reportName + "_" + timestamp + fileExtension;		
		}
		
		generateUserSpecifiedReportFormat(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID, reportType, 
				liveDirPath + "/" + inputFileName, outputFileName, liveDirPath, workAreaDirName, liveDirPath + "/" + xslFileName, 
				formatXSL, whereTarget, whereTagExpr, whereTextSubStr, prevTimestamp, currTimestamp);

		return MRXMLUtils.stringToDocument("<ExportReportInfo>" +
											  "<Name>" + reportName + "</Name>" +
											  "<WorkAreaDir>" + workAreaDirPath + "</WorkAreaDir>" +	
											  "<LiveDir>" + liveDirPath + "</LiveDir>" +
											  "<OutputFile>" + workAreaDirPath + "/" + outputFileName + "</OutputFile>" +
											  "<DestinationFile>" + destFileName + "</DestinationFile>" +
											  "<EnableExport>" + enableExport + "</EnableExport>" +
											  "<ExportURL>" + exportURL + "</ExportURL>" +
											  "<CurrentDate>" + currentDate + "</CurrentDate>" +
											  "<CurrentTime>" + currentTime + "</CurrentTime>" +		
										  "</ExportReportInfo>");
				
	}
	
	private long getTimeInSecondsSinceEpoch(SimpleDateFormat simpleDateFormat, String date, String time) throws Exception {		
		Date dateObj = simpleDateFormat.parse(date + " " + time);
		long timeInMilliSecs = dateObj.getTime();
		return timeInMilliSecs/1000;
	}
	
	private void generateUserSpecifiedReportFormat(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory, String NID, String reportType, String inputFilePath, String outputFileName, 
			String liveDirPath, String workAreaDirName, String xslFilePath, String formatXSL, String whereTarget, String whereTagExpr, 
			String whereTextSubStr, String prevTimestamp, String currTimestamp) throws Exception
	{	
		log.addInfo("ReportManager::Generate IPVS custom report format and then apply XSL on it to produce user specified report format", MRLog.INFO, MRLog.NOTIFY);
			
		String rawPolicyXML = "";
		if (reportType.isEmpty()) {	
			throw new MRException(SessionServiceExceptionCodes.INVALID_REPORT_TYPE, "Cannot run report with NID = " + NID + ". Invalid report type");		
		} else if (reportType.equalsIgnoreCase(SessionServiceConst.POLICY)) {		
			rawPolicyXML = generatePolicyXMLDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, inputFilePath, whereTarget, whereTagExpr, whereTextSubStr);				
		}
		
		log.addInfo("ReportManager::Run script to generate the output report for report with NID = " + NID, MRLog.OK, MRLog.NOTIFY);
		executeReportProcessor(NID, reportType, inputFilePath, xslFilePath, outputFileName, formatXSL, rawPolicyXML, liveDirPath, 
				workAreaDirName, prevTimestamp, currTimestamp);		
	}

	private String generatePolicyXMLDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String inputFilePath, String whereTarget, String whereTagExpr, String whereTextSubStr) throws Exception
	{	
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + whereTarget + SessionServiceConst.XQUERYXML;
		
		List<String> listOfWhereXpath = new ArrayList<String>();	
		String modelObject = "";
			
		if (whereTarget.equals(SessionServiceConst.USER)) {
			String xpath = "//Groups/Tag[@Name!='_RESERVED_' and @Value!='DEVICE_BOT_USERS']";			
			listOfWhereXpath.add(xpath);
			modelObject = AMConst.User;
		}
		else { 		// Resource 			
			String xpath = "//Type [.= 'IPVSClient-Device' or .='WebClient-Device']";			
			listOfWhereXpath.add(xpath);
			modelObject = AMConst.AssetResource;
		}
				
		Document targetListDoc = GetObjectUtils.getQueryList(request, xmpp, log, db, serviceAgentFactory, resourceFactory, 0, 1000, "", "", SessionServiceConst.SELECTFORMAT_EXTENDED, "", "", whereTagExpr, whereTextSubStr,"", "", listOfWhereXpath, xqueryFileName, whereTarget, modelObject, null,null);
		
		addPermissions(targetListDoc, whereTarget, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		// Generate UserPolicyReport/ResourcePolicyReport document
		String policyType = whereTarget + SessionServiceConst.POLICY;
		String xml = "<" + policyType + SessionServiceConst.REPORT_OBJECT_TYPE + "/>";
		Document targetPolicyReportDoc = MRXMLUtils.getDocument(xml);
		Element targetListEl = MRXMLUtils.newElement(policyType + SessionServiceConst.LIST);
		MRXMLUtils.addElement(targetPolicyReportDoc.getRootElement(), targetListEl);
		MRXMLUtils.addChildElement(targetListEl, targetListDoc.getRootElement());		
		return targetPolicyReportDoc.asXML();
	}
	
	private synchronized void executeReportProcessor(String NID, String reportType, String inputFilePath, String xslFilePath, String outputFileName, 
			String formatXSL, String rawPolicyXML, String liveDirPath, String workAreaDirName, String prevTimestamp, String currTimestamp) throws Exception
	{
		// Write content to input file 
		if (reportType.equalsIgnoreCase(SessionServiceConst.POLICY))
			FileUtils.writeFile(inputFilePath, rawPolicyXML);
		
		// Write content to xsl file
		if (formatXSL.isEmpty()) {		
			throw new MRException(SessionServiceExceptionCodes.MISSING_FORMAT_XSL, "Missing format XSL");
		} else {
			FileUtils.writeFile(xslFilePath, formatXSL);	
		}
		
		String[] systemCmd = {"sh", "/etc/scripts/preliminary_report_processing.sh", reportType, outputFileName, liveDirPath, workAreaDirName, prevTimestamp, currTimestamp};
		if(MediaRoomServiceUtils.executeSystemCommand(systemCmd) != 0)				
			throw new MRException(SessionServiceExceptionCodes.PRELIMINARY_REPORT_PROCESSING_COMMAND_FAILED, "Preliminary report processing command failed for NID = " + NID);
	}
		
	public void addPermissions(Document targetRequestDoc, String objectType, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		Document matchedDocumentPolicies = null;
		Document reportInstanceDoc = MRXMLUtils.loadXMLResource(USER_POLICY_REPORT_INSTANCE_XML, log);
		Element permissionsEL = MRXMLUtils.getElementXpath(reportInstanceDoc, "//" + SessionServiceConst.PERMISSIONS);
		List<?> viewObjectTypeList = MRXMLUtils.getChildren(permissionsEL);
		List<?> objectList = MRXMLUtils.getListXpath(targetRequestDoc, "//" + objectType);
		
		// objects: User, Resource 
		for(Object object:objectList){
			Element objectEl = (Element)object;
			
			Element operatorEl = MRXMLUtils.newElement("Operator");
			Element objectTypeEl = MRXMLUtils.newElement(objectType);
			MRXMLUtils.addElement(objectTypeEl, (Element)objectEl.clone());
			MRXMLUtils.addElement(operatorEl, objectTypeEl);
			Document policyRequestDoc = MRXMLUtils.elementToDocument(operatorEl);
		        
			Element permissionElement = MRXMLUtils.newElement(SessionServiceConst.PERMISSIONS);
			
			//viewObjectTypes: Port, Device, Dir, File, SessionConfig, User, Profile
			for(Object viewObjectType:viewObjectTypeList) {
				Element viewObjectTypeEl = ((Element)viewObjectType);
				viewObjectTypeEl = (Element)viewObjectTypeEl.clone();
				List<String> policyRoleObjectList = SessionObjectPermissions.getPolicyRoleObjectListForGivenViewObject(log, viewObjectTypeEl.getName());
				matchedDocumentPolicies = PolicyManager.getMatchingTargetPolicies(policyRequestDoc, policyRoleObjectList, objectType, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				viewObjectTypeEl = checkObjectPermissions(matchedDocumentPolicies, policyRoleObjectList, viewObjectTypeEl, policyRequestDoc, objectType, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				MRXMLUtils.addElement(permissionElement, viewObjectTypeEl);				
			}
			MRXMLUtils.addElement(objectEl, permissionElement);
		}
	}
	
	public Element checkObjectPermissions(Document matchedDocumentPolicies, List<String> policyRoleObjectList, Element viewObjectTypeEl, Document policyRequestDoc, String objectType, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		List<String> actionList = MRXMLUtils.getValuesXpath(MRXMLUtils.elementToDocument(viewObjectTypeEl), "//" + SessionServiceConst.Action + "/@" + SessionServiceConst.NAME);
		String policyRoleXpath = "";	
		//for each action check policy satisfying or not.
		for(String action:actionList) {
			String delim="";
			StringBuffer policyRoleObjectStr = new StringBuffer();
			for(String policyRoleObject : policyRoleObjectList) {

				//if the action is view from and object is not src port then skip the check 
				if(action.equals(PolicyAdminConstants.VIEW_FROM_ROLE_ACTION) && !policyRoleObject.equals(AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE)) {
					continue;
				}

				//if the action is stream to and object is not dst port then skip the check
				if(action.equals(PolicyAdminConstants.STREAM_TO_ROLE_ACTION) && !policyRoleObject.equals(AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE)) {
					continue;
				}
				
				policyRoleObjectStr.append(delim + "("+ SessionServiceConst.ROLEOBJECT+"='"+policyRoleObject+"')");
				delim=" or ";
			}
			
			if(!policyRoleObjectStr.toString().isEmpty())
			  policyRoleXpath = "//PolicyRule[./RoleList/*/*" +
					"["+"("+policyRoleObjectStr.toString()+") and "+ 
					SessionServiceConst.ROLEACTION+"='"+action+"'"+"]]";

			List<?> matchedPolicyRuleList = MRXMLUtils.getListXpath(matchedDocumentPolicies, policyRoleXpath);
			if(matchedPolicyRuleList != null && !matchedPolicyRuleList.isEmpty()) {
				for(Object policyRule:matchedPolicyRuleList) {
					Element policyRuleEl = (Element)policyRule;
					String targetXpath = TargetObjectLib.getTargetXpath(policyRequestDoc, policyRuleEl, objectType);
					Document getTagsResponse = getAllTagsBasedOnViewObjectType(request, viewObjectTypeEl.getName(), xmpp, log, db, resourceFactory, serviceAgentFactory);
					if((targetXpath != null) && !targetXpath.isEmpty()) {
						//(.//Groups/Tag[@Name='Department']/@Value ='Engineering')
						String regex = "Tag.*?Name\\s*=\\s*'(.*?)'.*?Value\\s*(.*?)\\s*'(.*?)'";
						Matcher matcher = Pattern.compile(regex).matcher(targetXpath);
						while(matcher.find()) {
							String operator = matcher.group(2);
							String name= matcher.group(1);
							String value = matcher.group(3);
							// Checks if same tag is already existing for that particular action. Adds if it is not present.
							if(MRXMLUtils.getElementXpath(viewObjectTypeEl, "./Action[@name='"+action+"']/Groups/Tag[@Name='"+matcher.group(1)+"' and @Value='"+matcher.group(3)+"']") == null) {
									String xPath = "//Tag[.[@Name='"+name+"']/@Value"+operator+" '"+ value +"']";
									List<?> tagList = MRXMLUtils.getListXpath(getTagsResponse, xPath);
									MRXMLUtils.addChildrenToElement(MRXMLUtils.getElementXpath(viewObjectTypeEl, "./Action[@name='"+action+"']/Groups"), tagList);
							}
						}
					}
					else if(targetXpath.isEmpty()) {
						//Gets all tags if targetXpath is empty
						List<?> tagsList = MRXMLUtils.getListXpath(getTagsResponse, "//Tag");
						Element groupsEl = MRXMLUtils.getElementXpath(viewObjectTypeEl, "./Action[@name='"+action+"']/Groups");
						MRXMLUtils.addChildrenToElement(groupsEl, tagsList);
					} 
				}
			}	
		}
		return viewObjectTypeEl;
	}
	
	protected Document getAllTagsBasedOnViewObjectType(MRRequest request, String viewObjectType, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		String key = viewObjectType + "." + "Tags";
		
		if(request.getCache() == null)
			request.setCache(new MRRequestCache());
		
		MRRequestCache cache = request.getCache();
		if(cache.containsKey(key)) {
			return (Document)cache.get(key);
		}

		if(viewObjectType.equals(SessionServiceConst.PORT))
			viewObjectType = SessionServiceConst.PORTS;
		else if(viewObjectType.equals(SessionServiceConst.DEVICE))
			viewObjectType = SessionServiceConst.DEVICE;
		else if(viewObjectType.equals(SessionServiceConst.FILE) || viewObjectType.equals(SessionServiceConst.DIR))
			viewObjectType = SessionServiceConst.MEDIA;
		else if(viewObjectType.equals(SessionServiceConst.USER))
			viewObjectType = SessionServiceConst.USERS;
		else if(viewObjectType.equals(SessionServiceConst.PROFILE))
			viewObjectType = SessionServiceConst.PROFILES;
		else if(viewObjectType.equals(SessionServiceConst.SESSIONCONFIG))
			// TBD 
			viewObjectType = SessionServiceConst.ALL;
		
		Document getTagsResponse = getTagsResponseDoc(request, viewObjectType, xmpp, log, db, resourceFactory, serviceAgentFactory);
		MRXMLUtils.removeNodeXpath(getTagsResponse, "//Tag/@objectType");
		if(cache != null)
			cache.put(key, getTagsResponse);
	
		
		return (Document)cache.get(key);
		
	}
	
	protected Document getTagsResponseDoc(MRRequest userRequest, String viewObjectType, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		// gets Tag information 
		
		Document getTagsSampleRequestDoc = MRXMLUtils.loadXMLResource(GET_TAGS_REQUEST_XML, log);
		String requestName = MRRequest.getRequestName(GetTagsRequestHandler.class.getName());
		
		Element getTagsEL = MRXMLUtils.getElementXpath(getTagsSampleRequestDoc, "//GetTags");
		MRXMLUtils.setAttributeValue(getTagsEL, SessionServiceConst.WHERE_OBJECT_TYPE, viewObjectType);
		MRXMLUtils.setAttributeValue(getTagsEL, SessionServiceConst.SELECTFORMAT, SessionServiceConst.SELECTFORMAT_NAMEVALUELIST);
		
		Document getTagsRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, getTagsEL);
		MRRequest getTagsRequest = MRRequest.createRequest(userRequest, getTagsRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);	
		
		getTagsRequest.setSuperUser(true);
		MRRequestHandlerI TagsRequestHandler = new GetTagsRequestHandler();
		Document getTagsResponse = TagsRequestHandler.getResponse(getTagsRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		getTagsRequest.setSuperUser(false);
		return getTagsResponse;

	}

}
