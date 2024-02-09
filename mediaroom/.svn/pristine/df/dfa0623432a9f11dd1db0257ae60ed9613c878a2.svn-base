package com.ipvs.policyadminservice.impl;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.utils.FileUtils;

public class MediaRoomPolicyLib {
	public static final String MEDIA_ROOM_POLICY_LIST_QUERY = "/com/ipvs/policyadminservice/xquery/MediaRoomPolicyListQuery.xml";
	public static final String MEDIA_ROOM_POLICY_NAME="MediaRoomPolicy";
	public static final String SOURCE_XPATH ="SourceXpath";
	public static final String DESTINATION_XPATH ="DestinationXpath";
	public static final String SRC_RULE_KEY_LIST ="SrcRuleKeyList";
	public static final String DST_RULE_KEY_LIST ="DstRuleKeyList";

	private static Document mediaRoomPolicyDoc = null;
	private static MediaRoomPolicyLib mediaRoomPolicyLibObject = null;
	
	public static MediaRoomPolicyLib getMediaRoomPolicyLibObject() {
		if(mediaRoomPolicyLibObject == null) {
			mediaRoomPolicyLibObject = new MediaRoomPolicyLib();
		}
		return mediaRoomPolicyLibObject;
	}
	
	/*
	Query output Format
    <PolicyRule>
	     <OperatorXpath/>
	     <SourceXpath/>
	     <DestinationXpath/>
	     <RoleList/>
	     <RuleKeyList>
	        <SrcRuleKeyList/>
	        <DstRuleKeyList/>
	     </RuleKeyList>
	   </PolicyRule>
	 */
	
	public synchronized Document getMediaRoomPolicyDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		if(mediaRoomPolicyDoc == null) {
			Hashtable<String, String> table = new Hashtable<String, String>();
			
			String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
			FileUtils.readFileResource(MediaRoomPolicyLib.MEDIA_ROOM_POLICY_LIST_QUERY, request.getClass());			
			String policyXML = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, table, null);
		
			log.addInfo("Read Policy From DB. PolicyName :="+MEDIA_ROOM_POLICY_NAME);
			if(policyXML == null) 
				return null;
			mediaRoomPolicyDoc = MRXMLUtils.stringToDocument(policyXML);
		}
		
		return mediaRoomPolicyDoc;
	}
	
	public synchronized void removeCache() {
		mediaRoomPolicyDoc = null;
	}
	
	/**
	 * Returns the list of matching connections policies for given user request document.
	 * UserRequestDocument can be <RequestData><Operator><User/><Resource/></Operator><RoleAction>Action</RoleAction><Source><User/><Resource/></Source></RequestData>
	 * UserRequestDocument can be <RequestData><Operator><User/><Resource/></Operator><RoleAction>Action</RoleAction><Destination><User/><Resource/></Destination></RequestData>
	 * UserRequestDocument can be <RequestData><Operator><User/><Resource/></Operator><RoleAction>Action</RoleAction><Source><User/><Resource/></Source><Destination><User/><Resource/></Destination></RequestData>
	 * @param userRequestDoc
	 * @param modelObjectList
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return
	 * @throws MRException
	 * @throws Exception
	 */
	
	public  Document getMatchingMediaRoomPolicies(Document userRequestDoc,MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		Document matchedPolicesList = MRXMLUtils.getDocument();
		matchedPolicesList.setRootElement(MRXMLUtils.addElement(PolicyManager.POLICY_RULE_LIST));

		Document targetPoliciesDoc = getMediaRoomPolicyDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		List<?> policiesList = MRXMLUtils.getListXpath(targetPoliciesDoc, "//"+PolicyManager.POLICY_RULE);
		if(policiesList == null || policiesList.isEmpty()){
			return null;
		}
		Element policyRuleEL = null;
		String operatorXpath = null;
		String sourceXpath = null;
		String destinationXpath = null;
		
		boolean isOperatorMatching = false;
		boolean isSourceMatching = false;
		boolean isDestinationMatching = false;
		boolean isRoleActionMatching = false;
		Element requestDocSourceEL = MRXMLUtils.getElementXpath(userRequestDoc, "//"+PolicyAdminConstants.SOURCE);
		Element requestDocDestinationEL = MRXMLUtils.getElementXpath(userRequestDoc, "//"+PolicyAdminConstants.DESTINATION);
		Element requestDocRoleActionEL = MRXMLUtils.getElementXpath(userRequestDoc, "//"+PolicyAdminConstants.ROLE_ACTION);
		String roleActionXpath  = "";
		String roleObject = PolicyAdminConstants.MEDIA_ROOM_ROLE_OBJECT;
		String roleAction = MRXMLUtils.getValue2(requestDocRoleActionEL, "");
		roleActionXpath  = ".//*["+PolicyAdminConstants.ROLE_OBJECT+"='"+roleObject+"' and "+PolicyAdminConstants.ROLE_ACTION+"='"+roleAction+"']"; 
		
		for(Object object : policiesList) {
			
			isOperatorMatching = false;
			isSourceMatching = false;
			isDestinationMatching = false;
			isRoleActionMatching = false;
		
			policyRuleEL = (Element)((Element) object).clone();
			
			//if operator xpath  is empty means ALL filter or if the given request is matching with operator xpath then set the operator matching true
			operatorXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./" + PolicyManager.OPERATOR_XPATH, "");
			if(operatorXpath.isEmpty() ||  MRXMLUtils.getElementXpath(userRequestDoc,"//"+PolicyAdminConstants.OPERATOR + "[" + operatorXpath +"]") != null) {
				isOperatorMatching = true;
			}
			
			//if source xpath  is empty means ALL filter or if the user requesting policies with out source (example: getsourcexpath) or
			//if the given request is matching with source xpath then set the source matching true			
			sourceXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+ SOURCE_XPATH, "");		
			if(sourceXpath.isEmpty() || requestDocSourceEL == null || MRXMLUtils.getElementXpath(userRequestDoc,"//"+PolicyAdminConstants.SOURCE + "[" + sourceXpath +"]") != null) {
				isSourceMatching = true;
			}
			
			//if destination xpath  is empty means ALL filter or if the user requesting policies with out destinations (example: getdestinationxpath or getpvrorrecorddestinationxpath) or
			//if the given request is matching with destination xpath then set the destination matching true
			destinationXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+ DESTINATION_XPATH, "");		
			if(destinationXpath.isEmpty() || requestDocDestinationEL == null || MRXMLUtils.getElementXpath(userRequestDoc,"//"+PolicyAdminConstants.DESTINATION + "[" + destinationXpath +"]") != null) {
				isDestinationMatching = true;
			}
			
			//if the given role action is matching with policy then set role action true			
			if(roleAction.isEmpty() || MRXMLUtils.getElementXpath(policyRuleEL, roleActionXpath) != null) {
				isRoleActionMatching = true;
			}
			boolean isRuleKeyMatched =  matchRuleKeyWithRequestDoc(policyRuleEL,userRequestDoc,requestDocSourceEL,requestDocDestinationEL, request,  xmpp,  log,  db, resourceFactory,serviceAgentFactory);

			//if operator, source ,destination and role action matches then add policy to matching policy document 
			if(isOperatorMatching && isSourceMatching && isDestinationMatching && isRoleActionMatching && isRuleKeyMatched){
				MRXMLUtils.addElement(matchedPolicesList.getRootElement(), (Element)policyRuleEL.clone());
			}
		}
		
		return matchedPolicesList;
	}
	
	private boolean matchRuleKeyBasedOnRequestElement(Element policyRuleEL, Document userRequestDoc,String ignoreString, String xpathPrefix,String ruleKeyPrefix,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		 String prefixXPath  = "//" + xpathPrefix ;
		List<?> ruleKeyList = MRXMLUtils.getListXpath(policyRuleEL, ".//"+ruleKeyPrefix +"/"+ PolicyManager.RULE_KEY);
		if(ruleKeyList ==null || ruleKeyList.isEmpty()){
			return true;
		}
		
		Element ruleKeyEL = null;
		String lhTagXpath="";
		String operator="";
		String rhTagXpath="";
		String xpath = "";
		String lhKey = "";
		boolean isMatch = true;
		for (Object object : ruleKeyList) {
			ruleKeyEL = (Element) object;
			lhKey = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.LH_KEY, "");
			lhTagXpath = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.LH_TAG_XPATH, "");
			operator = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.OPERATOR, "");
			rhTagXpath = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.RH_TAG_XPATH, "");	
			if(!ignoreString.isEmpty() && rhTagXpath.contains(ignoreString)) {
				continue;
			}
			xpath = "(" + prefixXPath +"/" + lhKey +"/" + lhTagXpath + operator + rhTagXpath + ")"; 
			if(!MRXMLUtils.applyXPath(userRequestDoc, xpath)) {
				isMatch = false;
			}
		}
		return isMatch;
	}

	private  boolean matchRuleKeyWithRequestDoc(Element policyRuleEL, Document userRequestDoc,Element requestDocSourceEL, Element requestDocDestinationEL,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception {
		
		if(requestDocDestinationEL == null && requestDocSourceEL ==null) {
			return true;
		}
		boolean destinationMathed = true; 		
		if(requestDocDestinationEL != null) {
			//ignore sources for destination check if the request document does not have source
			String ignoreString = "";
			if(requestDocSourceEL == null) {
				ignoreString = PolicyAdminConstants.SOURCE;
			}
			destinationMathed = matchRuleKeyBasedOnRequestElement(policyRuleEL, userRequestDoc, ignoreString, PolicyAdminConstants.DESTINATION, DST_RULE_KEY_LIST , request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		
		boolean sourceMatched = true;
		if(requestDocSourceEL != null) {
			//ignore destination for source check if the request document does not have destination
			String ignoreString = "";
			if(requestDocDestinationEL == null) {
				ignoreString = PolicyAdminConstants.DESTINATION;
			}
			sourceMatched = matchRuleKeyBasedOnRequestElement(policyRuleEL, userRequestDoc, ignoreString, PolicyAdminConstants.SOURCE, SRC_RULE_KEY_LIST , request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		//if both source and destinations are matching then return true 
		return (sourceMatched && destinationMathed);
	}	
	
	public String getTargetXpath(String ruleKeyElXpath,String targetXPathElName,Document userRequestDoc,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		String targetXpath = "";
		Element policyRuleEL= null;
		String rulekeyXpath = "";
		Document matchingPoliciesDoc = getMatchingMediaRoomPolicies(userRequestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);		
		List<?> policyRuleList = MRXMLUtils.getListXpath(matchingPoliciesDoc, "//"+PolicyManager.POLICY_RULE);
		if(policyRuleList.isEmpty()) {
			return PolicyAdminConstants.NULL;
		}
		
		StringBuffer targetXpathBuff = new StringBuffer();
		String delim = "";
		for(Object object : policyRuleList) {
			policyRuleEL = (Element) object;
			targetXpath = MRXMLUtils.getValueXpath(policyRuleEL, "./"+targetXPathElName, null);
			targetXpath = PolicyManager.parseTargetXPath(targetXpath);
			if(targetXpath == null) {
				continue;
			}
			List<?> ruleKeyList = MRXMLUtils.getListXpath(policyRuleEL,ruleKeyElXpath+"["+PolicyManager.LH_KEY+"='"+PolicyAdminConstants.RESOURCE+"']");	
			rulekeyXpath = PolicyManager.generateRuleKeyXpath(userRequestDoc, policyRuleEL,ruleKeyList);
			
			if(rulekeyXpath!=null   && !rulekeyXpath.equals(PolicyAdminConstants.NULL)) {
				if(!targetXpath.isEmpty()) {
					targetXpath =  "("+targetXpath+")" + " and "+ "("+ rulekeyXpath +")";
				}
				else {
					targetXpath = "("+ rulekeyXpath +")";
				}
			}

			//target xpath empty means all filter. Get all the tags from database
			if(targetXpath!= null && targetXpath.isEmpty()) {
				return "";
			}
			targetXpathBuff.append(delim + "( " + targetXpath +" )" );
			delim =" or ";
		}
		targetXpath = targetXpathBuff.toString();
		if(targetXpathBuff.toString().isEmpty()){
			return null;
		}		
		return targetXpathBuff.toString();	
	}
	
}
