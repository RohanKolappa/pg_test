package com.ipvs.policyadminservice.impl;

import java.util.ArrayList;
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
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;

public class TargetObjectLib {

	public static final String TARGET_OBJECT_LIB_QUERY = "/com/ipvs/policyadminservice/xquery/TargetObjectLibQuery.xml";
	

	/**
	 * Returns the target request document for logged in user jid.
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return return the operator document
	 * @throws MRException
	 * @throws Exception
	 */
	/*
	 * output xml format
	 * <TargetRequestDoc>
	   <Operator>
	     <User/>
	     <Resource/>
	   </Operator>
	   </TargetRequestDoc>
	 */
	public static Document createRequestDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		Element operator = request.getOperatorGroups();
    	if(operator != null) {
    		Element targetRequestEl = MRXMLUtils.addElement("TargetRequestDoc");
    		MRXMLUtils.addElement(targetRequestEl,(Element)operator.clone());
    		return MRXMLUtils.elementToDocument(targetRequestEl);
    	}
    	
		String loginUserJID = request.getUserJID();		
		String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
		                FileUtils.readFileResource(TARGET_OBJECT_LIB_QUERY, request.getClass());
		query = query.replace(SessionServiceConst.USER_JID, loginUserJID);
		query = query + "\n let $requestDoc := local:createRequestDoc() \n  return $requestDoc ";
		
		Hashtable<String,String> pemXpathList = new Hashtable<String, String>();
		String targetRequestXML = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, pemXpathList, null);
		if(targetRequestXML == null) 
			return null;
		Document requestDoc =  MRXMLUtils.stringToDocument(targetRequestXML);		
		request.setOperatorGroups(MRXMLUtils.getElementXpath(requestDoc, "//"+PolicyAdminConstants.OPERATOR));		
		return  MRXMLUtils.stringToDocument(targetRequestXML);	
	}
	
	
	public static String getTargetXpath(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory,List<String> objectList, String wherePermissions, String roleAction) throws MRException,Exception {
		String targetXPath = PolicyAdminConstants.NULL;
		Document targetRequestDoc = TargetObjectLib.createRequestDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		Document matchedPolicesDoc=  PolicyManager.getMatchingTargetPolicies(targetRequestDoc,objectList,request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		List<?> matchedPolicesList = 	MRXMLUtils.getListXpath(matchedPolicesDoc, "//"+PolicyManager.POLICY_RULE);
		if(matchedPolicesList == null || matchedPolicesList.isEmpty()) {
			return PolicyAdminConstants.NULL;
		}
		if(wherePermissions.isEmpty())
			return getTargetXpath(targetRequestDoc, matchedPolicesList);
		
		try {
			targetXPath = PolicyAdminServiceUtils.getTargetXPathForGivenPermissions(matchedPolicesDoc, objectList, "", roleAction, wherePermissions, request,  xmpp,  log, db,  serviceAgentFactory,
					resourceFactory);
		}
		
		catch(Exception e){
			e.printStackTrace();
		}
		return targetXPath;
	}
	
	public static List<?> getMatchingTargetPoliciesBasedOnRole(Document matchingPoliciesList,	List<?> targetObjectRoleList, MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception {

		String delim = "";
		Element targetObjectRoleEL = null;
		StringBuffer policyXpathbuffer = new StringBuffer();

		for(Object object:targetObjectRoleList) {
			targetObjectRoleEL = (Element) object;
			String roleObject = MRXMLUtils.getValueXpath(targetObjectRoleEL, "./" + PolicyAdminConstants.ROLE_OBJECT, "");
			String roleAction = MRXMLUtils.getValueXpath(targetObjectRoleEL, "./" + PolicyAdminConstants.ROLE_ACTION, "");
			policyXpathbuffer.append(" " + delim +" ("+PolicyAdminConstants.ROLE_OBJECT+"='"+roleObject+"' and "+PolicyAdminConstants.ROLE_ACTION+"='"+roleAction+"')" ); 
			delim = " or ";
		}

		if(!policyXpathbuffer.toString().isEmpty()) {
			String roleXpath  =  "["+policyXpathbuffer.toString()+"]";
			return  MRXMLUtils.getListXpath(matchingPoliciesList, "//"+PolicyManager.POLICY_RULE + "[.//*"+roleXpath+"]");
		}

		return null;
	}
	
	/**
	 * Generates the targetXpath list from given matching policies.
	 * If the polices contain any empty targetXPath then it return empty value.
	 * Empty xPath means ALL Filter.
	 * @param requestDoc
	 * @param policyRuleList
	 * @return targetXpath which applies on target object
	 * @throws MRException
	 * @throws Exception
	 */
	public static String getTargetXpath(Document requestDoc,List<?> policyRuleList) throws MRException,Exception {
		StringBuffer targetXpathBuff = new StringBuffer();
		String delim = "";
		String targetXpath =null;

		Element policyRuleEL= null;
		for(Object object : policyRuleList) {
			policyRuleEL = (Element) object;
			targetXpath = getTargetXpath(requestDoc, policyRuleEL);
			//target xpath empty means all filter. Get all the tags from database
			if(targetXpath!= null && targetXpath.isEmpty()) {
				return "";
			}
			//remove the policy if it not matching with user information
			if(targetXpath.equals(PolicyAdminConstants.NULL)){
				policyRuleEL.detach();
				continue;
			}
			
			targetXpathBuff.append(delim + "( " + targetXpath +" )" );
			delim =" or ";
		}
		targetXpath = targetXpathBuff.toString();
		if(targetXpathBuff.toString().isEmpty()){
			return PolicyAdminConstants.NULL;
		}		
		return targetXpathBuff.toString();
	}
	
	public static String getTargetXpath(Document requestDoc, Element policyRuleEL) throws MRException,Exception {
		return getTargetXpath(requestDoc, policyRuleEL, "");
	}
	
	public static String getTargetXpath(Document requestDoc, Element policyRuleEL, String objectType) throws MRException,Exception {
		String targetXpath = MRXMLUtils.getValueXpath(policyRuleEL, ObjectPolicyLib.TARGET+"/"+ObjectPolicyLib.XPATH, null);	
		List<?> ruleKeyList = MRXMLUtils.getListXpath(policyRuleEL, ObjectPolicyLib.TARGET+"/"+PolicyManager.RULE_KEY_LIST+"/"+PolicyManager.RULE_KEY);	
		String rulekeyXpath = PolicyManager.generateRuleKeyXpath(requestDoc, policyRuleEL, ruleKeyList, objectType);		
		if(rulekeyXpath!=null && !rulekeyXpath.equals(PolicyAdminConstants.NULL)) {
			if(!targetXpath.isEmpty()) {
				targetXpath =  "("+targetXpath+")" + " and "+ "("+ rulekeyXpath +")";
			}
			else {
				targetXpath = "("+ rulekeyXpath +")";
			}
		}else if(rulekeyXpath!=null && rulekeyXpath.equals(PolicyAdminConstants.NULL)){
			return PolicyAdminConstants.NULL;
		}
		return targetXpath;
	}

	
	public static void enforceAccessPolicies(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory,Document targetDocument,String roleObject,String roleAction) throws MRException,Exception {
		Document targetRequestDoc = createRequestDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		List<String> modelObjectList = new ArrayList<String>();
		modelObjectList.add(roleObject);
		String roleXpath = PolicyAdminConstants.ROLE_OBJECT+"='"+roleObject+"' and "+PolicyAdminConstants.ROLE_ACTION+"='"+roleAction+"'";
		Document matchingPoliciesDoc = PolicyManager.getMatchingTargetPolicies(targetRequestDoc, modelObjectList, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		List<?> policyRuleList = MRXMLUtils.getListXpath(matchingPoliciesDoc, "//"+PolicyManager.POLICY_RULE + "[.//*["+roleXpath+"]]");
		if(policyRuleList == null || policyRuleList.isEmpty()) {
			throw new MRException(PolicyManagementExceptionCodes.ACCESS_POLICY_ACCESS_DENY, "Access Policy Access Denied");
		}
		
		for(Object policyRuleObject:policyRuleList){
			Element policyRuleEl = (Element)policyRuleObject;
			String targetXpath = TargetObjectLib.getTargetXpath(targetRequestDoc, policyRuleEl);
			if((targetXpath.isEmpty()) || (MRXMLUtils.applyXPath(targetDocument, targetXpath))) {
				return;
			}
		}
		
		throw new MRException(PolicyManagementExceptionCodes.ACCESS_POLICY_ACCESS_DENY, "Access Policy Access Denied");
	}
}
