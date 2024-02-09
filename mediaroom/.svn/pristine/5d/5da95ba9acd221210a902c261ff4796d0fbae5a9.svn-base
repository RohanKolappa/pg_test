package com.ipvs.policyadminservice.impl;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;

public class PolicyManager {	
	
	public static final String POLICY_QUERY_PATH = "/com/ipvs/policyadminservice/xquery/";
	//policy constants
	public static final String POLICY_RULE ="PolicyRule";
	public static final String OPERATOR_XPATH = "OperatorXpath";
	public static final String OPERATOR_USER_XPATH = "OperatorUserXpath";
	public static final String OPERATOR_RESOURCE_XPATH = "OperatorResourceXpath";
	public static final String NAME ="Name";
	public static final String POLICY = "Policy";
	public static final String ROLE_OBJECT = "RoleObject";
	public static final String POLICY_RULE_LIST ="PolicyRuleList";

	public static final String POLICY_TO_OBJECT_MAPPING = "/com/ipvs/policyadminservice/datamodel/PoliciesToRoleObjectsMapping.xml";
	public static final String POLICY_ROLE_OBJECT_MODEL_OBJECT_MAPPING = "/com/ipvs/policyadminservice/datamodel/PolicyRoleObjectToModelObjectMapping.xml";
	public static Document policyToObjectMappingDocument = null;
	public static Document policyRoleObjectToModelObjectMappingDocument = null;
	public static final String POLICY_ROLE_OBJECT="PolicyRoleObject";
	public static final String MODEL_OBJECT="ModelObject";
	public static final String MODEL_OBJECT_LIST ="ModelObjectList";
	public static final String TYPE="type";
	
	public static final String RULE_KEY_LIST = "RuleKeyList";
	public static final String RULE_KEY = "RuleKey";
	public static final String LH_TAG_XPATH = "LHTagXpath";
	public static final String LH_KEY = "LHKey";
	public static final String OPERATOR = "Operator";
	public static final String RH_TAG_XPATH = "RHTagXpath";

	
	private static ObjectPolicyLib objectPolicyLibObject = ObjectPolicyLib.getObjectPolicyLibObject();
	private static MediaRoomPolicyLib mediaRoomPolicyLibObject = MediaRoomPolicyLib.getMediaRoomPolicyLibObject();
	private static MediaStreamFilterLib mediaStreamFilterLibObject = MediaStreamFilterLib.getMediaStreamFilterLibObject();
	private static NetworkAddressFilterLib networkAddressFilterLibObject = NetworkAddressFilterLib.getNetworkAddressFilterLibObject();
	
	public static synchronized void handlePolicyChanged(String policyName,ServiceAgentFactoryI serviceAgentFactory) {
		
		//if the policy object is mediaroom policy then remove connection policy from cache
		if(policyName.equals(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE)){
			mediaRoomPolicyLibObject.removeCache();
		} else if (policyName.equals(PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE)) {			
			mediaStreamFilterLibObject.removeCache();
			StreamPolicyUtils.handlePolicyChanged();
		} else  if(policyName.equals(PolicyAdminConstants.NETWORK_ADDRESS_RULE)){
			networkAddressFilterLibObject.removeCache();
		} else if(policyName.indexOf("Admin") > 0) {
			objectPolicyLibObject.removeCache(policyName);
		} else if(policyName.equals(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE)) {
			mediaRoomPolicyLibObject.removeCache();
			mediaStreamFilterLibObject.removeCache();
			networkAddressFilterLibObject.removeCache();
			objectPolicyLibObject.cleanCache();
			StreamPolicyUtils.handlePolicyChanged();
			
			//Clear the SubscriberPolicy cache
			AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).clearSubscriberPolicyCache();
			
		}
	}
	
	public static List<String> getPolicyName(List<String> modelObjectList,MRLog log) throws MRException,Exception {

		if(policyToObjectMappingDocument == null) {
			policyToObjectMappingDocument = MRXMLUtils.loadXMLResource(POLICY_TO_OBJECT_MAPPING, log);
		}
		
		List<String> policyNameList = new ArrayList<String>();
		for(String modelObjectType : modelObjectList) {
			String policyXpath = "//"+POLICY+"["+ROLE_OBJECT+"='"+modelObjectType+"']";
			Element policyEL = MRXMLUtils.getElementXpath(policyToObjectMappingDocument, policyXpath);
			if(policyEL != null) {
				String policyName = MRXMLUtils.getAttributeValue(policyEL, NAME,"");
				if(!policyNameList.contains(policyName))
					policyNameList.add(policyName);
			}
		}
		return policyNameList;
	}
	
	public static List<String> getPolicyRoleObjectListForGivenModelObject(String modelObject,MRLog log) throws MRException,Exception {

		if(policyRoleObjectToModelObjectMappingDocument == null) {
			policyRoleObjectToModelObjectMappingDocument = MRXMLUtils.loadXMLResource(POLICY_ROLE_OBJECT_MODEL_OBJECT_MAPPING, log);
		}
		String policyXpath = "//"+POLICY_ROLE_OBJECT+"["+MODEL_OBJECT_LIST+"/"+MODEL_OBJECT+"='"+modelObject+"']/@"+TYPE;
		return MRXMLUtils.getValuesXpath(policyRoleObjectToModelObjectMappingDocument, policyXpath);				
	}
	
	
	public static String parseTargetXPath(String targetXpath) throws Exception {
		
		//Replace User and Resource with empty string
		String regex = "(User*)\\s+(and)\\s+(Resource)";
		Matcher matcher = Pattern.compile(regex).matcher(targetXpath);
        if(!matcher.find()) {
			return null;
		}
		String replacedString = matcher.replaceAll(".");
		
		return replacedString;
	}
	
	/**
	 * Generates the rule key xPath for target list
	 * @param requestedDoc
	 * @param policyRuleEL
	 * @return target ruleKey xPath which applies on target object
	 * @throws MRException
	 * @throws Exception
	 */
	
	public static String generateRuleKeyXpath(Document requestedDoc,Element policyRuleEL,List<?> ruleKeyList) throws MRException,Exception {
		return generateRuleKeyXpath(requestedDoc, policyRuleEL, ruleKeyList, "");
	}
	
	public static String generateRuleKeyXpath(Document requestedDoc, Element policyRuleEL, List<?> ruleKeyList, String objectType) throws MRException,Exception {
		if(ruleKeyList== null || ruleKeyList.isEmpty()) {
			return null;
		}		
		Element ruleKeyEL = null;
		String lhTagXpath = null;
		String rhTagXpath = null;
		String operator = null;
		List<String> rhValues = null;
		List<String> ruleKeyXpathList = new ArrayList<String>();
		StringBuffer ruleKeyXpathB = null;
		String delim = "";
		for(Object object: ruleKeyList) {
			ruleKeyEL = (Element) object;			
			lhTagXpath = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.LH_TAG_XPATH, "");
			operator = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.OPERATOR, "");
			rhTagXpath = MRXMLUtils.getValueXpath(ruleKeyEL, PolicyManager.RH_TAG_XPATH, "");
			rhValues= MRXMLUtils.getValuesXpath(requestedDoc, rhTagXpath);			
			if((!objectType.isEmpty() && !rhTagXpath.contains(objectType))){
				continue;
			}
			
			//if user does not have any matching values for given rule key then return null  
			if(rhValues==null){
				return PolicyAdminConstants.NULL;
			}
			
			delim = "";
			ruleKeyXpathB = new StringBuffer();
			for(String rhValue : rhValues){
				ruleKeyXpathB.append(delim + "("+lhTagXpath+" "+ operator +" '"+rhValue +"' )");
				delim = " or "; 
			}			
			if(!ruleKeyXpathB.toString().isEmpty())
				ruleKeyXpathList.add(ruleKeyXpathB.toString());
		}

		if(ruleKeyXpathList.isEmpty()){
			return null;
		}
		StringBuffer ruleKeyXPathBuffer = new StringBuffer("");
		delim = "";

		for (String xpath : ruleKeyXpathList) {
			ruleKeyXPathBuffer.append(" " + delim + " ");
			ruleKeyXPathBuffer.append("( " + xpath + ")");
			delim = " and ";
		}	
		return ruleKeyXPathBuffer.toString();
	}
	
	
	public static Document getMatchingMediaRoomPolicies(Document userRequestDoc,MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		return mediaRoomPolicyLibObject.getMatchingMediaRoomPolicies(userRequestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	public static Document getMatchingTargetPolicies(Document targetRequestDoc,List<String> modelObjectList, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
	   return objectPolicyLibObject.getMatchingTargetPolicies(targetRequestDoc, modelObjectList, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	public static Document getMatchingTargetPolicies(Document targetRequestDoc,List<String> modelObjectList, String objectType, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
	   return objectPolicyLibObject.getMatchingTargetPolicies(targetRequestDoc, modelObjectList, objectType, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	} 
	public static String getDestinationXpath(Document userRequestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception {		
		String ruleKeyElXpath =  "./"+PolicyManager.RULE_KEY_LIST +"/"+MediaRoomPolicyLib.DST_RULE_KEY_LIST +"/"+ PolicyManager.RULE_KEY;
		return mediaRoomPolicyLibObject.getTargetXpath(ruleKeyElXpath, MediaRoomPolicyLib.DESTINATION_XPATH, userRequestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);	
	}
	
	public  static String getSourceXpath(Document userRequestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception {		
		String ruleKeyElXpath =  "./"+PolicyManager.RULE_KEY_LIST +"/"+MediaRoomPolicyLib.SRC_RULE_KEY_LIST +"/"+ PolicyManager.RULE_KEY;
		return mediaRoomPolicyLibObject.getTargetXpath(ruleKeyElXpath, MediaRoomPolicyLib.SOURCE_XPATH, userRequestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);	
	}
	
	public static Document getRelayAndProfileXpath(String relayType,Document userRequestDoc,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		return mediaStreamFilterLibObject.getRelayAndProfileXpath(relayType, userRequestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
		
	public static Document executeCreateRequestDocQuery(String queryFileName,String dstNID,String srcNID,String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		String loginUserJID = request.getUserJID();
	
		String sourceUserKey = srcNID +"."+PolicyAdminConstants.USER;
		String destinationUserKey = dstNID +"."+PolicyAdminConstants.USER;
		
		//look src user,source resource,dst user, dst resource and operator in the request cache. 
		//If request cache contains those elements reuse other wise run the query to get the info and put them in cache
    
		MRRequestCache cache = request.getCache();
		if((cache != null)&& request.getOperatorGroups() != null && 
				(srcNID.isEmpty() || (cache.containsKey(sourceUserKey) && cache.containsKey(srcNID)))
				&& (dstNID.isEmpty() || (cache.containsKey(destinationUserKey) && cache.containsKey(dstNID)))) {			
			Document document = null;
			document = MRXMLUtils.getDocument("<RequestData></RequestData>");
    		MRXMLUtils.addElement(document.getRootElement(),(Element)request.getOperatorGroups().clone());
    		if(!srcNID.isEmpty()){
    			MRXMLUtils.addElement(document.getRootElement(),generatePolicyGroupsElement(PolicyAdminConstants.SOURCE, (Element)cache.get(sourceUserKey), ((Document)cache.get(srcNID)).getRootElement()));
    		}
    		if(!dstNID.isEmpty()) {
    			MRXMLUtils.addElement(document.getRootElement(),generatePolicyGroupsElement(PolicyAdminConstants.DESTINATION, (Element)cache.get(destinationUserKey), ((Document)cache.get(dstNID)).getRootElement()));
    		}
    		if(!roleAction.isEmpty()) {
    			MRXMLUtils.addElement(document.getRootElement(),MRXMLUtils.addElement("RoleAction", roleAction));
    		}
    		// System.out.println("Generate policy request doc from cache:");
    		return document;
		}
    	 	
    	
		String query = FileUtils.readFileResource(queryFileName, request.getClass());
		query = query.replace(SessionServiceConst.USER_JID, loginUserJID);
		query = query.replace(GetSourceLib.DST_NID, dstNID);
		query = query.replace(SessionServiceConst.SRC_NID, srcNID);
		query = query.replace(SessionServiceConst.ROLE_ACTION, roleAction);
		
		request.setSuperUser(true);
		Document requestDoc  = GetObjectUtils.executeQuery(query, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
		request.setSuperUser(false);
		
		if(cache != null) {
    		Element operatorEl = MRXMLUtils.getElementXpath(requestDoc, "//Operator");
    		if(operatorEl!=null)
    			request.setOperatorGroups(operatorEl);
    		updateGroupInfoInCache(request, PolicyAdminConstants.SOURCE, requestDoc);
    		updateGroupInfoInCache(request,PolicyAdminConstants.DESTINATION, requestDoc);
    		System.out.println("update policy request doc to cache:");
		}
		return requestDoc;	
	}
	
	public static Element getNetworkAddressRule(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		Element fromPortResource, Element toPortResource) throws Exception {
		return networkAddressFilterLibObject.getNetworkAddressRule(request, xmpp, log, db, fromPortResource, toPortResource);
	}
	
	public static Element generatePolicyGroupsElement(String groupName,Element userEl,Element resourceEl) throws MRException,Exception {
		
		Element groupEl = MRXMLUtils.newElement(groupName);
		MRXMLUtils.addElement(groupEl,PolicyAdminConstants.USER);
		MRXMLUtils.addElement(groupEl,PolicyAdminConstants.RESOURCE);
		
		if(userEl != null) {
			MRXMLUtils.addElement(MRXMLUtils.getElement(groupEl, PolicyAdminConstants.USER),(Element)userEl.clone());
		}
		
		if(resourceEl!=null) {
			MRXMLUtils.addElement(MRXMLUtils.getElement(groupEl, PolicyAdminConstants.RESOURCE),(Element)resourceEl.clone());
		}
		
		return groupEl;
	}
	
	
	public static void  updateGroupInfoInCache(MRRequest request, String groupName,Document resultDoc) throws MRException,Exception {
		
		Element groupEl = MRXMLUtils.getElementXpath(resultDoc, "//"+groupName);
		if(groupEl ==null) {
			return;			
		}		
		
		Element resource = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(groupEl, "./"+PolicyAdminConstants.RESOURCE));	
		String resourceNIDFromDB =MRXMLUtils.getAttributeValue( resource,SessionServiceConst.NID);
		
		if(resource!=null) {
			Document resourceDoc = MRXMLUtils.elementToDocument(resource);
			request.getCache().put(resourceNIDFromDB, resourceDoc);
			
			Element resourceUserElement = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(groupEl, "./"+PolicyAdminConstants.USER));
			if(resourceUserElement ==null) {
				resourceUserElement = MRXMLUtils.stringToElement("<"+PolicyAdminConstants.USER+"/>");
			}
			
			request.getCache().put(resourceNIDFromDB+"."+PolicyAdminConstants.USER, resourceUserElement);
		}
		
				
	}
	
} 
