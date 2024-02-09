package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

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
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetStreamProfileListRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
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
        // Check the data
        try {
            Element data = MediaRoomServiceUtils.cloneDataElement(request);
            String sourceNID = MRXMLUtils.getValue(data,"sourceNID","");
            Element operatorGroups = MediaRoomServiceUtils.getOperator(request, xmpp, log, db, resourceFactory,
                    serviceAgentFactory);
            Element destGroups = null;
            Element sourceGroups = null;

            String destNID = "";
            Element destElement = MRXMLUtils.getElementXpath(data, "//destNID");

            if (destElement != null) {
                destNID = MRXMLUtils.getValue(destElement);
            }

            if (!"".equals(destNID.trim())) {
                destGroups = MediaRoomServiceUtils.getDestination(request, destNID, xmpp, log, db,
                        resourceFactory, serviceAgentFactory);
            } else {
                destGroups = (Element) operatorGroups.clone();
            }

            if (!"".equals(sourceNID.trim())) {
                sourceGroups = MediaRoomServiceUtils.getSource(request, sourceNID, xmpp, log, db,
                        resourceFactory, serviceAgentFactory);
            } else {
                sourceGroups = (Element) operatorGroups.clone();
            }

            //System.out.println(MRXMLUtils.elementToString(sourceGroups));
            Document doc = MRXMLUtils.stringToDocument("<data><GetProfileXPathRequestData>" +
                    "<OperatorList/><Action>Add</Action><ObjectType/><RoomGroup><Tag Name = \"Location\" Value = \"Stanford\" /></RoomGroup><DestinationList/><SourceList />" +
                    "</GetProfileXPathRequestData></data>");
            MRXMLUtils.addElementXpath(doc, "//OperatorList",operatorGroups);
            MRXMLUtils.addElementXpath(doc, "//DestinationList",destGroups);
            MRXMLUtils.addElementXpath(doc, "//SourceList",sourceGroups);

            Document policyResponse = MediaRoomServiceUtils.getPolicyAdminResponse(request, xmpp, log, db, resourceFactory,
                    serviceAgentFactory, request.getServiceName(), "GetProfileXPathRequest", doc);             
            String xpath = MRXMLUtils.getValue(policyResponse.getRootElement());

            if (xpath == null) {
                throw new MRException(MediaRoomServiceExceptionCodes.XPATH_ELEMENT_MISSING, "No Element found in PEM response xml");
            }

            String profileName = "";
            Element profileNameElement = MRXMLUtils.getElementXpath(data, "//profileName");

            if (profileNameElement != null) {
                profileName = MRXMLUtils.getValue(profileNameElement);
            }

            Element whereAssistXML = MRXMLUtils.stringToElement(
                    "<V2DMediaStreamProfile><Groups><Tag /></Groups></V2DMediaStreamProfile>");
            String whereXPath = "";

            //System.out.println(xpath);
            if (((xpath != null) && (xpath.length() == 0)) && !"".equals(profileName.trim())) {
                whereXPath = "//V2DMediaStreamProfile[Title='" + profileName + "']";
            } else if (((xpath != null) && (xpath.length() == 0)) && "".equals(profileName.trim())) {
                whereXPath = "//V2DMediaStreamProfile";
            } else if (!"".equals(profileName.trim())) {
                whereXPath = "//V2DMediaStreamProfile[Title='" + profileName + "' and " + xpath + "]";
            } else {
                whereXPath = "//V2DMediaStreamProfile[" + xpath + "]";
            }

            log.addInfo("GetStreamProfileList PEMWhereXPath="+whereXPath);
            Document response = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory,
                    serviceAgentFactory, request.getServiceName(), "V2DMediaStreamProfile_BASE_ALL_GET_LIST.xml", whereAssistXML,
                    whereXPath);
           /* Element source = MRXMLUtils.getElementXpath(response, "//V2DMediaStreamProfile");
            if(source==null){
                throw new MRException(MediaRoomServiceExceptionCodes.NO_MATCHING_PROFILE,"No Matching Stream Profile in DB");
            }*/
            return response;
        } catch (MRException e) {
            e.printStackTrace();
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(PolicyManagementExceptionCodes.PEM_GET_FAILED, exp.toString());

        }
    }
}
