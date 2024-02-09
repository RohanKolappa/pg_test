package com.ipvs.policyadminservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.policyadminservice.impl.PEMImplementation;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;

public class GetMediaStoreXpathRequestHandler implements MRRequestHandlerI {
	 private static String serviceName = ServiceClient.POLICYADMINSERVICE;

	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		 log.addTraceBegin(request);

	        int authType = 1;
	        int role = 2;
	        String token = "testCommonToken";
	        String authority = "testAuthority";
	        String roomPolicyNID = MRConst.NULL;
	        String streamPolicyNID = MRConst.NULL;
	        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID,
	                streamPolicyNID, token, authority);
	        log.addTraceEnd(request);

	        return auth;
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB schema, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {

		Document resDoc = MRXMLUtils.getDocument();

        Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
        MRXMLUtils.addElement(rootRes, "ObjectType", "");
        
        Document reqDoc =  request.getMessage().getDocument();

        List<?> matchingPolicyList = null;
        String xPath = "";
        
        Element parentEL = MRXMLUtils.getElementXpath(reqDoc, "//Data/PolicyManagementRequestData/GetMediaStoreXPathRequest");
        Element operatorListEL = parentEL.element("OperatorList");
        List<?> operatorList = operatorListEL.elements("Operator");

        //MRXMLUtils.getListXpath(parentEL, "//GetAssetXPathRequest/OperatorList/Operator");
        String action = MRXMLUtils.getValueXpath(parentEL, "//" + PolicyAdminConstants.ACTION, "");
        String roleObjectType = MRXMLUtils.getValueXpath(parentEL, "//" + PolicyAdminConstants.OBJECT_TYPE, "");
        matchingPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorList,
                PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE, action, roleObjectType, serviceName, request, schema, log);

        List<Element> policyList = null;

        if ((matchingPolicyList != null) && (matchingPolicyList.size() != 0)) {
            policyList = PEMImplementation.sortPolicesBasedOnOrder(matchingPolicyList);
            xPath = PEMImplementation.getMediaStoreAdminXPath(policyList, operatorList, serviceName, request, schema, log);
        } else {
            throw new MRException(PolicyAdminServiceUtils.getExceptionCode(action),
                MRXMLUtils.generateErrorXML("dbobjecttype", roleObjectType));
        }
        Element xpathEL = MRXMLUtils.newElement("XPath");
        if (xPath == null) {
            MRXMLUtils.setValue(xpathEL, "null");
        } else {
            MRXMLUtils.setValue(xpathEL, xPath);
        }
        MRXMLUtils.addElement(rootRes, xpathEL);
        resDoc.getRootElement().setName(request.getResponseDataName());

        return resDoc;
	}

}
