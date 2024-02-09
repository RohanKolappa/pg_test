package com.ipvs.sessionservice.impl;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.impl.TargetObjectLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;

public class SessionObjectPermissions {

	private static Document viewToModelObjectMappingDoc = null;
	public static final String VIEW_TO_MODEL_MAPPING_XML = "/com/ipvs/sessionservice/datamodel/ViewToPolicyRoleObjectMapping.xml";
	private static final String PERMISSIONS_EL_NAME="Permissions";

	private static final String FALSE = "false";
	private static final String TRUE = "true";
	private static final String ROLE_OBJECT ="RoleObject";
	private static final String ROLE_ACTION ="RoleAction";

	private static final String VIEW_OBJECT ="ViewObject";
	private static final String TYPE ="type";
	private static final String POLICY_ROLE_OBJECT ="PolicyRoleObject";
	private static final String POLICY_ROLE_ACTION ="PolicyRoleAction";


	//get list of policy role objects for given view object.
	public static List<String> getPolicyRoleObjectListForGivenViewObject(MRLog log,String viewObjectType)  throws MRException,Exception {
		if(viewToModelObjectMappingDoc == null) {
			viewToModelObjectMappingDoc = MRXMLUtils.loadXMLResource(VIEW_TO_MODEL_MAPPING_XML, log);
		}
		Element viewEL = MRXMLUtils.getElementXpath(viewToModelObjectMappingDoc, "//"+ VIEW_OBJECT +"[@"+TYPE+"='"+viewObjectType+"']");
		return MRXMLUtils.getValuesXpath(viewEL, ".//"+POLICY_ROLE_OBJECT);
	}


	//get the list of possible actions for given view object
	public static List<String> getPossibleActionsForGivenViewObject(MRLog log,String viewObjectType)  throws MRException,Exception {
		if(viewToModelObjectMappingDoc == null) {
			viewToModelObjectMappingDoc = MRXMLUtils.loadXMLResource(VIEW_TO_MODEL_MAPPING_XML, log);
		}
		Element viewEL = MRXMLUtils.getElementXpath(viewToModelObjectMappingDoc, "//"+ VIEW_OBJECT +"[@"+TYPE+"='"+viewObjectType+"']");
		return MRXMLUtils.getValuesXpath(viewEL, ".//"+POLICY_ROLE_ACTION);
	}


	// get the matching policies and generate view object permissions element
	public static Element getObjectPermissions(Document targetRequestDoc,String viewObjectType,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		List<String> policyRoleObjectList = getPolicyRoleObjectListForGivenViewObject(log, viewObjectType);
		Document matchedDocumentPolices = null;
		if(!request.isSuperUser()) {
		    matchedDocumentPolices =  PolicyManager.getMatchingTargetPolicies(targetRequestDoc,policyRoleObjectList, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		return  getObjectPermissions(matchedDocumentPolices, policyRoleObjectList, viewObjectType,viewObjectType, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	
	// Generate user requested permission element based on given matching policies and policy objects. 
	public static Element getObjectPermissions(Document matchedDocumentPolices,List<String> policyRoleObjectList,String viewObjectType,String permissionElName, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		//add all action attributes with default permissions false
		Element viewObjectEl = MRXMLUtils.addElement(permissionElName);

		List<String> actionList = getPossibleActionsForGivenViewObject(log, viewObjectType);
		String permission = FALSE;
		
		//if superuser set all the permission true
		if(request.isSuperUser()) {
			permission= TRUE;
		}
		
		//get matching policy for user
		for(String action : actionList) {
			MRXMLUtils.addAttribute(viewObjectEl, action.toLowerCase(), permission);
		}	
		
		//if superuser return view object
		if(request.isSuperUser()) {
			return viewObjectEl;
		}
		
		//for each action check policy satisfying or not.
		for(String action:actionList) {
			//boolean isMatch = true;
			//for each object check the permission exists or not. If any object don't have permission then set the permission to be false
			for(String policyRoleObject : policyRoleObjectList) {
				
				/*//if the action is view from and object is not src port then skip the check 
				if(action.equals(PolicyAdminConstants.VIEW_FROM_ROLE_ACTION) && !policyRoleObject.equals(AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE)) {
					if(policyRoleObjectList.size() == 1)
					     isMatch = false;
					continue;
				}
				
				//if the action is stream to and object is not dst port then skip the check
				if(action.equals(PolicyAdminConstants.STREAM_TO_ROLE_ACTION) && !policyRoleObject.equals(AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE)) {
					if(policyRoleObjectList.size() == 1)
					     isMatch = false;
					continue;
				}*/
				
				//if role xpath is not matching then set the action to be false
				String policyXpath = "//*["+"("+ROLE_OBJECT+"='"+policyRoleObject+"' and "+ ROLE_ACTION+"='"+action+"')"+"]"; 
				if(MRXMLUtils.getElementXpath(matchedDocumentPolices, policyXpath) != null) {
					MRXMLUtils.setAttributeValue(viewObjectEl, action.toLowerCase(), TRUE);
				}
			}	

			//if policy xpath matches then set the action to be true.
			/*if(isMatch) {
				MRXMLUtils.setAttributeValue(viewObjectEl, action.toLowerCase(), TRUE);
			} */
		}
		
		return  viewObjectEl;
	}

	public static void addObjectPermissions(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
	        MRResourceFactoryI resourceFactory, Document queryResult, Document matchingPoliciesDoc,String viewObjectType, String detail) throws MRException, Exception{
		if((!viewObjectType.equals(SessionServiceConst.PORT)) && (!viewObjectType.equals(SessionServiceConst.FILE)) && (!viewObjectType.equals(SessionServiceConst.DIR)))
			return;
		
		if(viewObjectType.equals(SessionServiceConst.PORT))
			addPortObjectPermissions(request, xmpp, log, db, serviceAgentFactory, resourceFactory, queryResult, matchingPoliciesDoc, viewObjectType, detail);
		else if(viewObjectType.equals(SessionServiceConst.FILE))
			addPermissionElementToModelObject(request, xmpp, log, db, serviceAgentFactory, resourceFactory, queryResult, matchingPoliciesDoc, viewObjectType, detail, SessionServiceConst.MEDIA_STREAM_FILE_RESOURCE);
		
		else
			addPermissionElementToModelObject(request, xmpp, log, db, serviceAgentFactory, resourceFactory, queryResult, matchingPoliciesDoc, viewObjectType, detail, SessionServiceConst.MEDIA_STREAM_DIR_RESOURCE);
	}
	
	public static void addPortObjectPermissions(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
	        MRResourceFactoryI resourceFactory, Document queryResult, Document matchingPoliciesDoc, String viewObjectType, String selectFormat) throws MRException, Exception{
		
		List<String> srcPortList = new ArrayList<String>();
		srcPortList.add(SessionServiceConst.MEDIA_STREAM_SRC_PORT_RESOURCE);
		
		List<String> dstPortList = new ArrayList<String>();
		dstPortList.add(SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE);
		
		List<String> relayPortList = new ArrayList<String>();
		relayPortList.add(SessionServiceConst.MEDIA_STREAM_RELAY_PORT_RESOURCE);
		
		List<String> storePortList = new ArrayList<String>();
		storePortList.add(SessionServiceConst.MEDIA_STORE_PORT_RESOURCE);
		
		List<?> portList = MRXMLUtils.getListXpath(queryResult, "//" + SessionServiceConst.PORT);

		for(Object portObject: portList){
			Element portEl = (Element)portObject;
			String portType = MRXMLUtils.getAttributeValue(portEl, SessionServiceConst.PORT_TYPE, "");
			List<String> policyRoleObjectList = null;
			
			if(portType.equals(SessionServiceConst.STREAM_SRC)){
				policyRoleObjectList = srcPortList;
			}
			else if(portType.equals(SessionServiceConst.STREAM_DST)){
				policyRoleObjectList = dstPortList;
			}
			else if(portType.equals(SessionServiceConst.STREAM_RELAY)){
				policyRoleObjectList = relayPortList;
			}
			else {
				policyRoleObjectList = storePortList;
			}
			addPermissionElementToObjectElement(request, xmpp, log, db, serviceAgentFactory, resourceFactory, policyRoleObjectList, portEl, viewObjectType, matchingPoliciesDoc, selectFormat);
		}
	
	}
	
	public static void addPermissionElementToModelObject(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
	        MRResourceFactoryI resourceFactory, Document queryResult, Document matchingPoliciesDoc, String viewObjectType, String selectFormat, 
	        String policyRoleObject) throws MRException, Exception{
		
		List<String> policyRoleList = new ArrayList<String>();
		policyRoleList.add(policyRoleObject);
		
		List<?> objectTypeList = MRXMLUtils.getListXpath(queryResult, "//" + viewObjectType );
		
		for(Object object:objectTypeList) {
			Element objectTypeEl = (Element)object;
			addPermissionElementToObjectElement(request, xmpp, log, db, serviceAgentFactory, resourceFactory, policyRoleList, objectTypeEl, viewObjectType, matchingPoliciesDoc, selectFormat);
		}
	}
	

	public static void addPermissionElementToObjectElement(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory, List<String> policyRoleObjectList, Element objectEL, 
			String viewObjectType, Document matchingPolicyDoc, String selectFormat) throws Exception {

		List<?> policyRuleList = MRXMLUtils.getListXpath(matchingPolicyDoc, "//PolicyRule");
		Element matchingPolicyListEL = MRXMLUtils.addElement(PolicyManager.POLICY_RULE_LIST);
		Document requestDoc = TargetObjectLib.createRequestDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		for(Object policyRuleObject:policyRuleList){
			Element policyRuleEl = (Element)policyRuleObject;
			String targetXpath = TargetObjectLib.getTargetXpath(requestDoc, policyRuleEl);
			if(targetXpath.equals(PolicyAdminConstants.NULL))
				continue;
			
			if((targetXpath.isEmpty()) || (MRXMLUtils.applyXPath(MRXMLUtils.elementToDocument(objectEL), targetXpath))) {
				MRXMLUtils.addElement(matchingPolicyListEL,(Element)policyRuleEl.clone());
			}
		}
		Element permissionEL = getObjectPermissions(MRXMLUtils.elementToDocument(matchingPolicyListEL), policyRoleObjectList, 
				viewObjectType, PERMISSIONS_EL_NAME, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		MRXMLUtils.addElement(objectEL, permissionEL);
		/*if(selectFormat.equals(SessionServiceConst.SELECTFORMAT_BASIC)) {
			MRXMLUtils.removeChild(objectEL, "Groups");
		}*/
	}
	
	
	//generate permission element for each view object
	public static Element getViewObjectsPermissions(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {		

		Document targetRequestDoc = TargetObjectLib.createRequestDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory);		
		Element permissionsEL = MRXMLUtils.addElement(PERMISSIONS_EL_NAME);		
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.USER, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.PORT, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.DEVICE, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.DIR, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.FILE, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.PROFILE, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc,SessionServiceConst.SESSIONCONFIG, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc, SessionServiceConst.POLICY,request, xmpp, log, db, resourceFactory, serviceAgentFactory));		
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc, SessionServiceConst.SYSTEM,request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		MRXMLUtils.addElement(permissionsEL,getObjectPermissions(targetRequestDoc, SessionServiceConst.REPORT_OBJECT_TYPE,request, xmpp, log, db, resourceFactory, serviceAgentFactory));
		return permissionsEL;
	}

}
