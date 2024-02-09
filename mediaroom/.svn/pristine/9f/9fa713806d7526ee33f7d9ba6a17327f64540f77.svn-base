package com.ipvs.policyadminservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.impl.TargetObjectLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;


public class GetTargetXPathRequestHandler implements MRRequestHandlerI {
  
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
    
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        Document requestDocument = request.getMessage().getDocument();

        Element operatorEl = MRXMLUtils.getElementXpath(requestDocument, "//"+PolicyAdminConstants.OPERATOR);
        List<String> objectList= MRXMLUtils.getValuesXpath(requestDocument, "//"+PolicyAdminConstants.ROLE_OBJECT);
        
        //find matching policies based on operator 
        Document matchingPoliciesDoc = PolicyManager.getMatchingTargetPolicies(operatorEl.getDocument(), objectList, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        
        //find matching policies list based on given object and action
        List<?> targetObjectRoleList = MRXMLUtils.getListXpath(requestDocument, "//"+PolicyAdminConstants.TARGET_OBJECT_ROLE);
        List<?> policyRuleList  = TargetObjectLib.getMatchingTargetPoliciesBasedOnRole(matchingPoliciesDoc,targetObjectRoleList,request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        Element targetXPathEl = MRXMLUtils.addElement(PolicyAdminConstants.TARGET_XPATH); 
         
        //if no matching policy set the xpath to null. null means no matching policy empty means allow ALL
        if(policyRuleList == null || policyRuleList.isEmpty()) {
        	MRXMLUtils.setValue(targetXPathEl, PolicyAdminConstants.NULL);
        } else {
        	MRXMLUtils.setValue(targetXPathEl,TargetObjectLib.getTargetXpath(operatorEl.getDocument(), policyRuleList));
        }
        
        //Add xpath and operator matching policies to responseDoc 
        Document responseDoc = request.createResponseData();
        Element operatorMatchingPoliciesEL = MRXMLUtils.addElement("OperatorMatchingPolicies");
        MRXMLUtils.addContent(operatorMatchingPoliciesEL, MRXMLUtils.getListXpath(matchingPoliciesDoc, "//"+PolicyManager.POLICY_RULE));
        MRXMLUtils.addElement(responseDoc.getRootElement(),operatorMatchingPoliciesEL);
        MRXMLUtils.addElement(responseDoc.getRootElement(),targetXPathEl);
        
        return responseDoc;
    }
}
