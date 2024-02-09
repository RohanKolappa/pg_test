package com.ipvs.policyadminservice.impl;

import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;

public class GetSourceLib {

	public static final String GET_SOURCE_LIB_QUERY = "/com/ipvs/policyadminservice/xquery/GetSourceLibQuery.xml";
	
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
	   </RequestData>
	 * @throws MRException
	 * @throws Exception
	 */
	// private static final String SOURCE = "SOURCE";
	 public static final String DST_NID ="_DSTNID_";
		
     private static Document createRequestDoc(String dstNID,String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
	 	return PolicyManager.executeCreateRequestDocQuery(GET_SOURCE_LIB_QUERY, dstNID, "",roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
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
	public static String  getSourceXpath(String dstNID , String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		Document requestDoc = createRequestDoc(dstNID, roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		String resourceXPath  = PolicyManager.getSourceXpath(requestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
	
		if (resourceXPath == null || resourceXPath.equals(PolicyAdminConstants.NULL)) {
			throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_SOURCES, "No Permission available to access Sources");
		}

		return resourceXPath.toString();
	}

}
