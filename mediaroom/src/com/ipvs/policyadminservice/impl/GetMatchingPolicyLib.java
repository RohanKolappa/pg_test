package com.ipvs.policyadminservice.impl;

import java.util.List;

import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;

public class GetMatchingPolicyLib {

	public static final String GET_MATCHING_POLICY_LIB_QUERY = "/com/ipvs/policyadminservice/xquery/GetMatchingPolicyLibQuery.xml";
	
	/**
	 * Create the request document for get destination xpath 
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return 
	 *	<RequestData>
				<Operator>
					<User/>
					<Resource/>
				</Operator>
				<RoleAction>Action</RoleAction>
				<Destination>
					<User/>
					<Resource/>
				</Destination>
				<Source>
					<User/>
					<Resource/>
				</Source>
	   </RequestData>
	 * @throws MRException
	 * @throws Exception
	 */
	// private static final String MATCHING_POLICY = "MATCHING_POLICY";
	 	
     private static Document createRequestDoc(String dstNID,String srcNID,String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {    	 
 		return PolicyManager.executeCreateRequestDocQuery(GET_MATCHING_POLICY_LIB_QUERY, dstNID, srcNID,roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	
	/**
	 * 
	 * @param dstNID
	 * @param roleAction
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
	public static Document  getMatchingPolicy(String dstNID ,String srcNID, String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		Document requestDoc = createRequestDoc(dstNID,srcNID, roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		Document matchingPoliciesDoc= PolicyManager.getMatchingMediaRoomPolicies(requestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		List<?> policyRuleList = MRXMLUtils.getListXpath(matchingPoliciesDoc, "//"+PolicyManager.POLICY_RULE);
		
		if (policyRuleList == null || policyRuleList.isEmpty()) {
			throw new MRException(PolicyManagementExceptionCodes.CONNECTION_POLICY_ACCESS_DENY, "Connection Policy Access Denied");
		}
		return matchingPoliciesDoc;
	}

}
