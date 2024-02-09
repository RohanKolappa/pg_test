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
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;

public class ObjectPolicyLib {

	public static final String TARGET_OBJECT_LIB_QUERY = "/com/ipvs/policyadminservice/xquery/TargetObjectLibQuery.xml";
	public static final String TARGET = "Target";
	public static final String XPATH = "XPath";
	public static final String RULE_KEY_XPATH = "./"+ ObjectPolicyLib.TARGET+"/"+PolicyManager.RULE_KEY_LIST+"/"+PolicyManager.RULE_KEY;
	public static final String TARGET_XPATH = "./"+ ObjectPolicyLib.TARGET+"/"+ObjectPolicyLib.XPATH;
	private static Hashtable<String, Document> targetPoliciesList = new Hashtable<String, Document>();
	private static ObjectPolicyLib objectPolicyLibObject= null;
	
	
	
	private ObjectPolicyLib (){}
	public static ObjectPolicyLib getObjectPolicyLibObject() {
		if(objectPolicyLibObject == null) {
			objectPolicyLibObject = new ObjectPolicyLib();
		}
		return objectPolicyLibObject;
	}


	public  synchronized void removeCache(String adminPolicyName){

		int adminIndex = adminPolicyName.indexOf("Admin");

		String targetPolicyName = adminPolicyName.substring(0, adminIndex)+"Policy";
		if(!ObjectPolicyLib.targetPoliciesList.containsKey(targetPolicyName)) {
			return;
		}
		MRLog.getInstance(MRLog.CONSOLE).addInfo("Remove PolicyCache.PolicyName:"+targetPolicyName);

		ObjectPolicyLib.targetPoliciesList.remove(targetPolicyName);
	}
	
	public  synchronized void cleanCache(){
		ObjectPolicyLib.targetPoliciesList.clear();
	}


	/**
	 *Check given policy exists in the db or not. If not then read from database
	 * @param targetPolicyName
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
	/*
	   Query output Format
	   <PolicyRule>
	     <OperatorXpath/>
	     <RoleList/>
	     <Target>
	      <XPath/>
	      <RuleKeyList/>
	     <Target/>
	   </PolicyRule>
	 */
	public synchronized Document getObjectPolicy(String targetPolicyName,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		Document adminPolicyDoc = ObjectPolicyLib.targetPoliciesList.get(targetPolicyName);
		if(adminPolicyDoc == null) {
			Hashtable<String, String> table = new Hashtable<String, String>();
			String policyQueryPath = PolicyManager.POLICY_QUERY_PATH+targetPolicyName+"ListQuery.xml";
			String query = 	FileUtils.readFileResource(PEMDBUtils.XQUERYLIB, request.getClass()) +
			FileUtils.readFileResource(TARGET_OBJECT_LIB_QUERY, request.getClass()) + 
			FileUtils.readFileResource(policyQueryPath, request.getClass());

			String policyXML = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, table, null);
			log.addInfo("Read Policy From DB. PolicyName :="+targetPolicyName);
			if(policyXML == null) 
				return null;
			adminPolicyDoc = MRXMLUtils.stringToDocument(policyXML);
			targetPoliciesList.put(targetPolicyName, adminPolicyDoc);
		}
		return adminPolicyDoc;
	}

	/**
	 * Returns the matching polices for given object
	 * @param targetRequestDoc
	 * @param objectType
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return list of policies matched
	 * @throws MRException
	 * @throws Exception
	 */
	public  Document getMatchingTargetPolicies(Document targetRequestDoc,List<String> modelObjectList,MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		return getMatchingTargetPolicies(targetRequestDoc, modelObjectList, "", request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	public  Document getMatchingTargetPolicies(Document targetRequestDoc, List<String> modelObjectList, String objectType, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		List<String> targetPolicyNameList = PolicyManager.getPolicyName(modelObjectList, log);
		Document matchedPolicesList = MRXMLUtils.getDocument();
		matchedPolicesList.setRootElement(MRXMLUtils.addElement(PolicyManager.POLICY_RULE_LIST));

		for(String targetPolicyName : targetPolicyNameList) {
			Document targetPoliciesDoc = getObjectPolicy(targetPolicyName, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
			List<?> policesList = MRXMLUtils.getListXpath(targetPoliciesDoc, "//"+PolicyManager.POLICY_RULE);
			if(policesList == null || policesList.isEmpty()){
				return null;
			}
			Element policyRuleEL = null;
			String operatorXpath = null;

			for(Object object : policesList) {
				policyRuleEL = (Element)((Element) object).clone();
				
				//match operator xpath with given operator document.
				operatorXpath = MRXMLUtils.getValueXpath(policyRuleEL, PolicyManager.OPERATOR_XPATH, "");
				
				if(objectType.equals(SessionServiceConst.USER)) {
					//get User Xpath
					operatorXpath = MRXMLUtils.getValueXpath(policyRuleEL, PolicyManager.OPERATOR_USER_XPATH, "");
				}
				else if(objectType.equals(SessionServiceConst.RESOURCE)) {
					//get Resource Xpath
					operatorXpath = MRXMLUtils.getValueXpath(policyRuleEL, PolicyManager.OPERATOR_RESOURCE_XPATH, "");
				}
				
				//special case for all filter
				if(operatorXpath.isEmpty()) {
					MRXMLUtils.addElement(matchedPolicesList.getRootElement(), (Element)policyRuleEL.clone());
					continue;
				}
				if(MRXMLUtils.getElementXpath(targetRequestDoc, operatorXpath) == null ) {
					continue;
				}
				MRXMLUtils.addElement(matchedPolicesList.getRootElement(), (Element)policyRuleEL.clone());
			}
		}
		return matchedPolicesList;
	}
	
	
}
