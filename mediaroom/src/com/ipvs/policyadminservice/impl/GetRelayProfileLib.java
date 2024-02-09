package com.ipvs.policyadminservice.impl;

import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;

public class GetRelayProfileLib {

	public static final String GET_RELAY_PROFILE_LIB_QUERY = "/com/ipvs/policyadminservice/xquery/GetRelayProfileLibQuery.xml";
	
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
	 //private static final String RELAY = "RELAY";
		
     public static Document createRequestDoc(String dstNID,String srcNID,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		return PolicyManager.executeCreateRequestDocQuery(GET_RELAY_PROFILE_LIB_QUERY, dstNID, srcNID,"", request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
     
     
     //Call the Policy Admin handler to get the PEM xPath
     public static Document getRelayProfileXPath(Document requestDoc, String relayType,MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
         throws MRException, Exception {
         return PolicyManager.getRelayAndProfileXpath(relayType,requestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
     }
}
