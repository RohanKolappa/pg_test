package com.ipvs.policyadminservice.impl;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class SessionPolicyUtils {





	public static void enforcePolicies(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document sessionDoc)  throws MRException,Exception {

		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//Connection");
		Element connectionEl = null;
		String srcEntityType = null;
		String dstEntityType = null;
		String srcEntityId = null;
		String dstEntityId =  null;
		Element srcEntityEL = null;
		Element dstEntityEL = null;
		String connectionType =  null;
		String srcResourceNID = null;
		String dstResourceNID = null;
		
		if (request.getUserJID().equals(request.getAppServerJID())) {
			return;
		}

		for(Object object : connectionList) {
			connectionEl = (Element) object;

			try {
				srcEntityId = MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.SRCENTITYID,"");
				if("".equals(srcEntityId))
					throw new MRException(SessionServiceExceptionCodes.SOURCE_ENTITY_ERROR,
					"srcEntityId not found in connection");
				dstEntityId = MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.DSTENTITYID,"");
				if("".equals(dstEntityId)) {
					throw	new MRException(SessionServiceExceptionCodes.DEST_ENTITY_ERROR,
					"dstEntityId not found in connection");
				}
				srcEntityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityId+"']");
				dstEntityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+dstEntityId+"']");
				String srcEntityState = MRXMLUtils.getAttributeValue(srcEntityEL, SessionServiceConst.STATE,"");
				String dstEntityState = MRXMLUtils.getAttributeValue(dstEntityEL, SessionServiceConst.STATE,"");
				if(!srcEntityState.isEmpty() || !dstEntityState.isEmpty()) {
					continue;
				}
				srcResourceNID = MRXMLUtils.getAttributeValueXpath(srcEntityEL, "./"+SessionServiceConst.PORT,SessionServiceConst.RESOURCENID , "");	
				connectionType= MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.TYPE_ATTRIBUTE,"");
				if(connectionType.equals(SessionServiceConst.PLAYBACK)) {
					srcResourceNID = MRXMLUtils.getAttributeValueXpath(srcEntityEL, "./"+SessionServiceConst.FILE,SessionServiceConst.RESOURCENID , "");
				} 
				srcEntityType = MRXMLUtils.getAttributeValue(srcEntityEL, SessionServiceConst.TYPE_ATTRIBUTE,"");
				dstResourceNID = MRXMLUtils.getAttributeValueXpath(dstEntityEL, "./"+SessionServiceConst.PORT,SessionServiceConst.RESOURCENID , "");
				dstEntityType = MRXMLUtils.getAttributeValue(dstEntityEL, SessionServiceConst.TYPE_ATTRIBUTE,"");

				enforcePolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcResourceNID, dstResourceNID, srcEntityType, dstEntityType, connectionType);
			} catch (MRException exp) {
				SessionServiceUtils.updateSetupError(log, connectionEl, exp);
			}catch(Exception exp){
				SessionServiceUtils.updateSetupError(log, connectionEl, exp);
			}

		}
	}

	public static void enforcePolicies(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory,String srcNID,String dstNID,String srcType,String dstType,String connectionType) throws MRException,Exception {

		//enforce source 
		enforceAccessPolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcNID,srcType);

		//enforce destination and connection policy if its view or playback. No need to enforce pvr or record. 
		//PVR and RECORD enforce in SessionResourceUtils.updateSessionDoc() method
		if(connectionType.equals(SessionServiceConst.VIEW) || connectionType.equals(SessionServiceConst.PLAYBACK)) {
			enforceAccessPolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstNID,dstType);

			//enforce VIEW and PLAYBACK connection policy. 
			String roleAction = "";
			if(connectionType.equals(SessionServiceConst.VIEW)) {
				roleAction = PolicyAdminConstants.VIEW_ROLE_ACTION;
			} else if(connectionType.equals(SessionServiceConst.PLAYBACK)) {
				roleAction = PolicyAdminConstants.PLAYBACK_ROLE_ACTION;
			} else {
				return;
			}

			//enforce Connection Policy
			GetMatchingPolicyLib.getMatchingPolicy(dstNID, srcNID, roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
	}

	public static void enforceAccessPolicies(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory,String resourceNID,String type) throws MRException,Exception {

		MRRequestCache requestCache = request.getCache();
		String policyObjectType = "";
		String action= "";
		Document resourceDoc = null;

		if(type.equals(SessionServiceConst.PLYFILE)) {
			policyObjectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
			action = PolicyAdminConstants.PLAYBACK_ROLE_ACTION;
		} else if(type.equals(SessionServiceConst.SRCPORT)) {
			policyObjectType = AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE;
			action = PolicyAdminConstants.VIEW_FROM_ROLE_ACTION;
		} else if(type.equals(SessionServiceConst.DSTPORT)) {
			policyObjectType = AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE;
			action = PolicyAdminConstants.STREAM_TO_ROLE_ACTION;
		} else {
			return;
		}

		resourceDoc = (Document)requestCache.get(resourceNID);
		if(resourceDoc == null) {
			resourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
		}

		TargetObjectLib.enforceAccessPolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, resourceDoc, policyObjectType, action);
	}

	public static void enforcePoliciesOnUpdateSession(MRRequest request,XMPPI xmpp,MRLog log,MRDB db,
			MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, Document sessionDoc,
			Element updateElement, String srcEntityId, String dstEntityId) throws MRException,Exception {
		
		String srcType = "";	
		String srcResourceNID = "";
		String srcResourceTitle = "";
		String dstType="";
		String dstResourceNID = "";
		String dstResourceTitle = "";
		Element resourceElement = null;
		String connectionType = SessionServiceConst.VIEW;
		if(!srcEntityId.isEmpty()) {
			Element entityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityId+"']");
			srcType = MRXMLUtils.getAttributeValue(entityEl, SessionServiceConst.TYPE_ATTRIBUTE,""); 
			if(srcType.equals(SessionServiceConst.SRCPORT)) {
			   resourceElement = MRXMLUtils.getElement(entityEl, SessionServiceConst.PORT);
			} else if(srcType.equals(SessionServiceConst.PLYFILE)) {
				resourceElement = MRXMLUtils.getElement(entityEl , SessionServiceConst.FILE);
				connectionType = SessionServiceConst.PLAYBACK;
			}
			srcResourceNID = MRXMLUtils.getAttributeValue(resourceElement, SessionServiceConst.RESOURCENID,"");
		} else {
			resourceElement = MRXMLUtils.getElementXpath(updateElement, ".//" + SessionServiceConst.SRCFILE);
			if (resourceElement == null) {
				resourceElement = MRXMLUtils.getElementXpath(updateElement, ".//"+SessionServiceConst.SRCPORT);
				srcResourceNID = MRXMLUtils.getAttributeValue(resourceElement, "portNID","");
				srcResourceTitle = MRXMLUtils.getAttributeValue(resourceElement, "portTitle","");
				srcType = SessionServiceConst.SRCPORT;
			} else {
				srcResourceNID = MRXMLUtils.getAttributeValue(resourceElement, "fileNID","");
				srcResourceTitle = MRXMLUtils.getAttributeValue(resourceElement, "fileTitle","");
				srcType = SessionServiceConst.PLYFILE;
				connectionType = SessionServiceConst.PLAYBACK;	
			}	
		}

		if(!dstEntityId.isEmpty()) {
			Element entityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+dstEntityId+"']");
			dstType = MRXMLUtils.getAttributeValue(entityEl, SessionServiceConst.TYPE_ATTRIBUTE,""); 
			if(dstType.equals(SessionServiceConst.DSTPORT)) {
				resourceElement = MRXMLUtils.getElement(entityEl, SessionServiceConst.PORT);
			} else {
				resourceElement = MRXMLUtils.getElement(entityEl, SessionServiceConst.FILE);
			}
			dstResourceNID = MRXMLUtils.getAttributeValue(resourceElement, SessionServiceConst.RESOURCENID,"");
		} else {
			resourceElement = MRXMLUtils.getElementXpath(updateElement, ".//" + SessionServiceConst.DSTPORT);
			if (resourceElement != null) {
				dstResourceNID = MRXMLUtils.getAttributeValue(resourceElement, "portNID","");
				dstResourceTitle = MRXMLUtils.getAttributeValue(resourceElement, "portTitle","");
				dstType = SessionServiceConst.DSTPORT;
			} else {
				dstType = SessionServiceConst.DST_FILE;
				connectionType = SessionServiceConst.RECORD;
			}
		}


		if((dstResourceNID.isEmpty() && dstType.equals(SessionServiceConst.DSTPORT)) || srcResourceNID.isEmpty()) {

			//update operator element
			Document queryDoc = MRXMLUtils.stringToDocument("<query/>");
			Element opratorQueryEl = SessionResourceUtils.createOperatorQueryElement(request);
			queryDoc.getRootElement().add(opratorQueryEl);			
			
			if(dstResourceNID.isEmpty() && dstType.equals(SessionServiceConst.DSTPORT) && !dstResourceTitle.isEmpty()) {
				Element qelement = MRXMLUtils.stringToElement("<queryitem " + SessionServiceConst.ELEMENTNAME + "='" + dstType + "'/>");
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCETITLE, dstResourceTitle);
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCENID, dstResourceNID);
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.TYPE_ATTRIBUTE, dstType);
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCEJID, "");				
				queryDoc.getRootElement().add(qelement);		
			}

			if(srcResourceNID.isEmpty()) {
				Element qelement = MRXMLUtils.stringToElement("<queryitem " + SessionServiceConst.ELEMENTNAME + "='" + srcType + "'/>");
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCETITLE, srcResourceTitle);
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCENID, srcResourceNID);
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.TYPE_ATTRIBUTE, srcType);
				MRXMLUtils.addAttribute(qelement,SessionServiceConst.RESOURCEJID, "");				
				queryDoc.getRootElement().add(qelement);
			}
			
			Document queryResultDoc =SessionResourceUtils.getQueryResult(request, xmpp, log, db, queryDoc);
			SessionResourceUtils.updateOperatorElementInCache(request, queryResultDoc);
			
			if(dstResourceNID.isEmpty() && dstType.equals(SessionServiceConst.DSTPORT) && !dstResourceTitle.isEmpty()) {
				resourceElement = MRXMLUtils.getElementXpath(queryResultDoc, "//resultitem[@"+SessionServiceConst.ELEMENTNAME+"='"+dstType+"']");
				Element resource = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourceElement, "./"+PolicyAdminConstants.RESOURCE));				
				dstResourceNID = MRXMLUtils.getAttributeValue(resource, SessionServiceConst.NID,"");
				if(dstResourceNID.isEmpty()) {
					throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, MRXMLUtils.getValueXpath(resourceElement, ".//data", "") 
					+ " not found");
				}
				SessionResourceUtils.updateResourceInfoInCache(request, resourceElement, dstType);
			}

			if(srcResourceNID.isEmpty()) {
				resourceElement = MRXMLUtils.getElementXpath(queryResultDoc, "//resultitem[@"+SessionServiceConst.ELEMENTNAME+"='"+srcType+"']");
				Element resource = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourceElement, "./"+PolicyAdminConstants.RESOURCE));
				srcResourceNID = MRXMLUtils.getAttributeValue(resource, SessionServiceConst.NID,"");
				if(srcResourceNID.isEmpty()) {
					throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, MRXMLUtils.getValueXpath(resourceElement, ".//data", "") 
					+ " not found");
				}
				SessionResourceUtils.updateResourceInfoInCache(request, resourceElement, srcType);
			}
		}
		
		enforcePolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcResourceNID, dstResourceNID, srcType, dstType, connectionType);
	}


	public static String  getDirNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory,String srcNID, String mediaType,String requestedDirNID,String targetAccessXpath)
	throws MRException, Exception {
		
		String roleAction = PolicyAdminConstants.RECORD_ROLE_ACTION;
		if(MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)) {			
			roleAction = PolicyAdminConstants.PVR_ROLE_ACTION;
		}

		//get pem xpath from policy service for given source
		String pemXPath = "";
		if (!request.getUserJID().equals(request.getAppServerJID())) {
			try {
				pemXPath = GetDestinationLib.getDestinationXpath(srcNID, roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
			} catch (MRException e) {
				throw new MRException(SessionServiceExceptionCodes.DIR_CONNECTION_POLICY_DENY, "Dir Connection Policy Deny");
			}
		}
		
		//merge access and connection policy xpath while checking for destination direcotry
		if( !pemXPath.isEmpty() && !targetAccessXpath.isEmpty()) {
			pemXPath = "//MediaStreamDirResource["+pemXPath+" and ."+targetAccessXpath+"]";
		} else if(!pemXPath.isEmpty() && targetAccessXpath.isEmpty()){
			pemXPath = "//MediaStreamDirResource["+pemXPath+"]";
		} else if(pemXPath.isEmpty() && !targetAccessXpath.isEmpty()){
			pemXPath = "//MediaStreamDirResource["+targetAccessXpath+"]";
		} else if(pemXPath.isEmpty() && targetAccessXpath.isEmpty()){
			pemXPath = "//MediaStreamDirResource";
		}

		String queryFilePath  = SessionServiceConst.XQUERYPACKAGE + "GetRecordOrPVRDirQuery.xml";
		String query = FileUtils.readFileResource(queryFilePath, request.getClass());
		query = query.replace(SessionServiceConst.DIRTYPE, roleAction);
		query = query.replace(SessionServiceConst.REQUESTEDDIRNID, requestedDirNID);

		//apply pem xpath on media stream dir
		Map<String, String> pemXpathList = new HashMap<String, String>();
		pemXpathList.put(AMConst.MEDIA_STREAM_DIR_RESOURCE, pemXPath);
		String queryResult = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, pemXpathList, null);

		if ((queryResult == null) || queryResult.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.OTHER, "Execution of Getdir Failed" );
		}

		Document responseDoc = MRXMLUtils.stringToDocument(queryResult);

		String parentDirNID  = "";

		MRRequestCache cache = request.getCache();

		List<String> listOfDir = null;
		
		List<?> dirList = MRXMLUtils.getListXpath(responseDoc, "//" + SessionServiceConst.DIR+ "/"+ AMConst.MEDIA_STREAM_DIR_RESOURCE);
		if(dirList.isEmpty() && requestedDirNID.isEmpty()){
			throw new MRException(SessionServiceExceptionCodes.DESTDIR_NOT_FOUND, "No Dir Found.Check Dir Access/Connection Policy or Dir Tags");
		}
		else if(dirList.isEmpty() && !requestedDirNID.isEmpty()){
			throw new MRException(SessionServiceExceptionCodes.CAN_NOT_ACCESS_GIVEN_DEST_DIR, "Can not access requested Dir. Check Connection Policy or Dir Tags");
		} else {
			
			//verify at least one dir in the list ready
			boolean isDirOffline = false;
			if(!MRXMLUtils.applyXPath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "/.//StateFlag='Ready'")) {
				isDirOffline = true;
				listOfDir = MRXMLUtils.getValuesXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[not(.//StateFlag='Ready')]/.//Title");

			}
			
			//verify at least one dir in the list writable
			boolean isDirNeedUprade = false;
			if(!MRXMLUtils.applyXPath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "/.//Permissions/Write='true'")) {
				isDirNeedUprade = true;
				listOfDir = MRXMLUtils.getValuesXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[.//Permissions/Write='false']/.//Title");
			}
			
			//check at least one dir in the list does not exceeded threshold
			boolean isDirThresholdExceeded = false;
			if(MRXMLUtils.getElementXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/" +AMConst.MEDIA_STREAM_DIR_RESOURCE + "[(.//Size - .//SizeUsed) >= .//MediaDirSpanning/@diskFullThreshold]") == null) {
				isDirThresholdExceeded = true;
				listOfDir = MRXMLUtils.getValuesXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[(.//Size - .//SizeUsed) < .//MediaDirSpanning/@diskFullThreshold]/.//Title");
			}
			
			
			String dirErrorDescriptionString = getDirErrorDescriptionString(listOfDir);
			if(isDirNeedUprade && isDirThresholdExceeded && isDirOffline) {
				throw new MRException(SessionServiceExceptionCodes.CAN_NOT_ACCESS_DIR, "Dir Offline, Need Upgrade and Threshold Exceeded " + dirErrorDescriptionString);
			} else if(isDirNeedUprade && isDirThresholdExceeded) {
				throw new MRException(SessionServiceExceptionCodes.CAN_NOT_ACCESS_DIR, "Dir Need Upgrade and Threshold Exceeded " + dirErrorDescriptionString);
			} else if(isDirOffline && isDirThresholdExceeded) {
				throw new MRException(SessionServiceExceptionCodes.CAN_NOT_ACCESS_DIR, "Dir Offline and Threshold Exceeded "+dirErrorDescriptionString);
			} else if(isDirOffline && isDirNeedUprade) {
				throw new MRException(SessionServiceExceptionCodes.CAN_NOT_ACCESS_DIR, "Dir Offline and Need Upgrade "+dirErrorDescriptionString);
			} else if(isDirNeedUprade) {
				throw new MRException(SessionServiceExceptionCodes.DIR_NEED_UPGRADE, "Dir Need Upgrade "+dirErrorDescriptionString);
			} else if(isDirThresholdExceeded) {
				throw new MRException(SessionServiceExceptionCodes.DIR_THRESHOLD_EXCEEDED, "Dir Threshold Exceeded "+dirErrorDescriptionString);
			} else if(isDirOffline) {
				throw new MRException(SessionServiceExceptionCodes.DIR_OFFLINE, "Dir Offline "+dirErrorDescriptionString);
			}
		}
		
		//pick the directory if the dir is ready and has write permissions and disk threshold not exeeced
		Element dirElement = null;
		if(MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)) {
			dirElement = MRXMLUtils.getElementXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "["
				  +AMConst.MEDIA_STREAM_DIR_RESOURCE + "[(.//StateFlag='Ready') and (.//Permissions/Write='true') " +
				"and  (.//Size - .//SizeUsed) >= .//MediaDirSpanning/@diskFullThreshold]]/PVRDir/"+AMConst.MEDIA_STREAM_DIR_RESOURCE);
		} else {
			 dirElement = MRXMLUtils.getElementXpath(responseDoc, "//"+ SessionServiceConst.DIR+ 
					 "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[(.//StateFlag='Ready') and  (.//Permissions/Write='true') " +
				"and  (.//Size - .//SizeUsed) >= .//MediaDirSpanning/@diskFullThreshold] ");
		}
		
		//no directory found which is online,writable and threshold not exceeded
		if(dirElement == null) {
			
			listOfDir = MRXMLUtils.getValuesXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[not(.//StateFlag='Ready')]/.//Title");
			String dirErrorDescriptionString =  "";
			if(listOfDir!=null && !listOfDir.isEmpty())
				dirErrorDescriptionString += "Dir Offline "+ getDirErrorDescriptionString(listOfDir)+ " or ";
			
			listOfDir = MRXMLUtils.getValuesXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[.//Permissions/Write='false']/.//Title");
			if(listOfDir!=null && !listOfDir.isEmpty())				
				dirErrorDescriptionString += "Need Upgrade" + getDirErrorDescriptionString(listOfDir) + " or ";
			
			listOfDir = MRXMLUtils.getValuesXpath(responseDoc, "//"+ SessionServiceConst.DIR+ "/"+AMConst.MEDIA_STREAM_DIR_RESOURCE + "[(.//Size - .//SizeUsed) < .//MediaDirSpanning/@diskFullThreshold]/.//Title");
			if(listOfDir!=null &&  !listOfDir.isEmpty())				
				dirErrorDescriptionString += " Threshold Exceeded" + getDirErrorDescriptionString(listOfDir);
			
			throw new MRException(SessionServiceExceptionCodes.CAN_NOT_ACCESS_DIR, dirErrorDescriptionString);
		}
		parentDirNID = MRXMLUtils.getAttributeValue(dirElement,"NID");
		if(cache != null) {
			cache.put(parentDirNID, MRXMLUtils.elementToDocument(dirElement));
		}
		
		return parentDirNID;

	}
	
	private static String getDirErrorDescriptionString(List<String> listOfValues) {
		return Utils.stringJoin("(", ")", listOfValues, ",");
	}

	public static String getDirAccessXPath(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String roleAction) {
		List<String> roleObjectList = new ArrayList<String>();
		roleObjectList.add(AMConst.MEDIA_STREAM_DIR_RESOURCE);
		if (request.getUserJID().equals(request.getAppServerJID())) {
			return "";
		}
		try {
			return PolicyAdminServiceUtils.getTargetXpath(roleObjectList, AMConst.MEDIA_STREAM_DIR_RESOURCE, roleAction, request, xmpp, log, 
					db, resourceFactory, serviceAgentFactory);
		}catch(Exception exp){
			return PolicyAdminConstants.NULL;
		}
	}
	
	//return pvr dir xpath 
	public static String getPVRAccessXpath(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
		ServiceAgentFactoryI serviceAgentFactory,Document sessionDoc) throws Exception {	    
		List<?> streamPVRFileList = MRXMLUtils.getListXpath(sessionDoc, "//Entity[@type='"+SessionServiceConst.PVRFILE+"']");
		if(streamPVRFileList !=null && !streamPVRFileList.isEmpty()) 
		   return getDirAccessXPath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, PolicyAdminConstants.PVR_ROLE_ACTION);
		else
		    return PolicyAdminConstants.NULL;		
	}
}
