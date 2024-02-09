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


public class GetAssetXPathRequestHandler implements MRRequestHandlerI {
    private static String serviceName = ServiceClient.POLICYADMINSERVICE;

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
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
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        //add response resDoc
        Document resDoc = MRXMLUtils.getDocument();

        Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
        MRXMLUtils.addElement(rootRes, "ObjectType", "");

        Document reqDoc = request.getMessage().getDocument();

        List<?> matchingPolicyList = null;
        String xPath = "";

        //try {
        Element parentEL = MRXMLUtils.getElementXpath(reqDoc, "//Data/PolicyManagementRequestData/GetAssetXPathRequest");
        Element operatorListEL = parentEL.element("OperatorList");
        List<?> operatorList = operatorListEL.elements("Operator");

        //MRXMLUtils.getListXpath(parentEL, "//GetAssetXPathRequest/OperatorList/Operator");
        String action = MRXMLUtils.getValueXpath(parentEL, "//" + PolicyAdminConstants.ACTION, "");
        String roleObjectType = MRXMLUtils.getValueXpath(parentEL, "//" + PolicyAdminConstants.OBJECT_TYPE, "");
        matchingPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorList,
                PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE, action, roleObjectType, serviceName, request, schema, log);

        List<Element> policyList = null;

        if ((matchingPolicyList != null) && (matchingPolicyList.size() != 0)) {
            policyList = PEMImplementation.sortPolicesBasedOnOrder(matchingPolicyList);
            xPath = PEMImplementation.getAssetAdminPolicyXpath(policyList, operatorList, serviceName, request, schema, log);
        } else {
            throw new MRException(PolicyAdminServiceUtils.getExceptionCode(action),
                MRXMLUtils.generateErrorXML("dbobjecttype", roleObjectType));
        }

        /*}catch (MRException ex) {
             ex.printStackTrace();
             //System.out.println("testing in mrwexception%%%%");
             throw new MRException(ex.getCode(), ex.getData());
         } catch (Exception ex) {
                 //System.out.println("testing in exception%%%%");
             ex.printStackTrace();
             throw new MRException(DBExceptionCodes.DB_GET_FAILED, ex.getMessage());
         }*/
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
