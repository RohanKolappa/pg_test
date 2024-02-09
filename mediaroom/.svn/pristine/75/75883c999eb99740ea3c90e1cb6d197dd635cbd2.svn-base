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

public class GetDestinationLib {

	public static final String GET_DESTINATION_LIB_QUERY = "/com/ipvs/policyadminservice/xquery/GetDestinationLibQuery.xml";
	
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
				<Action>Action</Action>
				<Source>
					<User/>
					<Resource/>
				</Source>
	   </RequestData>
	 * @throws MRException
	 * @throws Exception
	 */
	 //private static final String DESTINATION = "Destination";
     private static Document createRequestDoc(String srcNID,String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {		
		return PolicyManager.executeCreateRequestDocQuery(GET_DESTINATION_LIB_QUERY, "", srcNID,roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	
	/**
	 * Returns the pem resulted xpath 
	 * @param srcNID
	 * @param userXpath
	 * @param roleAction
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return PEM Xpath 
	 * @throws MRException
	 * @throws Exception
	 */
	public static String  getDestinationXpath(String srcNID , String roleAction,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		
		Document requestDoc = createRequestDoc(srcNID, roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		String resourceXPath  = PolicyManager.getDestinationXpath(requestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	
		if (resourceXPath == null || resourceXPath.equals(PolicyAdminConstants.NULL)) {
			throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_DESTINATIONS, "No Permission available to access Destinations");
		}

		return resourceXPath.toString();
	}

}
