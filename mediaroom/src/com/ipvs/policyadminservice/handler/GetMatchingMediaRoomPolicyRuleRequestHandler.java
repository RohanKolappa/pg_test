package com.ipvs.policyadminservice.handler;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.policyadminservice.impl.PEMImplementation;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetMatchingMediaRoomPolicyRuleRequestHandler implements MRRequestHandlerI {
    private static String serviceName = ServiceClient.POLICYADMINSERVICE;
    static final int DST_USER_AND_DST_RESOURCE = 1;
    static final int DST_USER_AND_SRC_USER = 2;
    static final int DST_USER_AND_SRC_RESOURCE = 3;
    static final int DST_RESOURCE_AND_SRC_USER = 4;
    static final int DST_RESOURCE_AND_SRC_RESOURCE = 5;
    static final int OPERATOR_USER_DST_USER = 6;
    static final int OPERATOR_USER_DST_RESOURCE = 7;
    static final int OTHER = 8;
    static final int OPERATOR_USER_AND_SRC_USER = 2;
    static final int OPERATOR_USER_AND_SRC_RESOURCE = 3;
    static final int OPERATOR_RESOURCE_AND_SRC_USER = 4;
    static final int OPERATOR_RESOURCE_AND_SRC_RESOURCE = 5;
    static final int OPERATOR_RESOURCE_DST_USER = 6;
    static final int OPERATOR_RESOURCE_DST_RESOURCE = 7;
    static final int DST_USER_AND_PROFILE = 1;
    static final int DST_RESOURCE_AND_PROFILE = 4;
    static final int SRC_USER_AND_PROFILE = 1;
    static final int SRC_RESOURCE_AND_PROFILE = 4;
    static final int SRC_USER_AND_SRC_RESOURCE = 9;

    /********Invalid Cases*************/
    //final int DST_USER_AND_DST_USER = 8;
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = 1;
        int role = 2;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    @SuppressWarnings("unchecked")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        //add response resDoc
        Document resDoc = MRXMLUtils.getDocument();

        try {
            List<Element> roomPolicyList = new ArrayList<Element>();

            if (request == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Request"));
            }

            Document xmlDoc = request.getMessage().getDocument();

            if (xmlDoc == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Request Document"));
            }

            //System.out.println(MRXMLUtils.documentToString(xmlDoc));
            MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, log);
            String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

            if (parentNID == null) {
                return MRXMLUtils.stringToDocument("<XPATH></XPATH>");
            }

            //Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE + "List");
            Element dataEL = MRXMLUtils.getElementXpath(xmlDoc, "//GetMatchingMediaRoomPolicyRuleRequestData");

            if (dataEL == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "GetMatchingRoomPolicyNIDData"));
            }

            Element roomGroup = dataEL.element("RoomGroup");
            String action = dataEL.elementText("Action");
            Element sourceList = dataEL.element("SourceList");

            if (sourceList == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Source List"));
            }

            List<Element> sourceElementList = sourceList.elements();
            Element destinationList = dataEL.element("DestinationList");

            if (destinationList == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Destination List"));
            }

            Element operatorList = dataEL.element("OperatorList");

            if (operatorList == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Operator List"));
            }

            List<Element> destinationElementList = destinationList.elements();
            List<Element> operatorElementList = operatorList.elements("Operator");

            List<?> mediaRoomPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorElementList, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, action, "MediaRoom", serviceName, request, db, log);

            if (mediaRoomPolicyList == null) {
                return (MRXMLUtils.stringToDocument("<XPathList> </XPathList>"));
            }

            for (int i = 0; i < mediaRoomPolicyList.size(); i++) {
                roomPolicyList.add((Element) mediaRoomPolicyList.get(i));
            }

            Element[][] destinationFilterElement = MediaRoomPolicyHelper.findMatchingDestinationGroupFilter_New(destinationElementList, serviceName, true, request, db, log);
            Element[][] sourceFilterElement = MediaRoomPolicyHelper.findMatchingDestinationGroupFilter_New(sourceElementList, serviceName, true, request, db, log);
            List<Element> destinationUserFilterList = null;
            List<Element> destinationResourceFilterList = null;
            List<Element> sourceUserFilterList = null;
            List<Element> sourceResourceFilterList = null;
            List<String> sourceXpath = new ArrayList<String>();
            List<String> destinationXpath = new ArrayList<String>();

            // List<String> profileXpath = new ArrayList();
            List<String> roomXpath = new ArrayList<String>();

            for (int i = 0; (i < 30) && (destinationElementList != null) && (destinationFilterElement != null); i++) {
                if (destinationFilterElement[i][0] == null) {
                    break;
                }

                destinationUserFilterList = destinationFilterElement[i][0].elements("TagObjectFilter");
                destinationResourceFilterList = destinationFilterElement[i][1].elements("TagObjectFilter");

                List<String> destinationUserGroupNidList = new ArrayList<String>();
                List<String> destinationResourceGroupNidList = new ArrayList<String>();

                for (int j = 0; (destinationUserFilterList != null) && (j < destinationUserFilterList.size()); j++) {
                    destinationUserGroupNidList.add(MRXMLUtils.getAttribute(destinationUserFilterList.get(j),"NID").getValue());
                }

                for (int j = 0; (destinationResourceFilterList != null) && (j < destinationResourceFilterList.size()); j++) {
                    destinationResourceGroupNidList.add(MRXMLUtils.getAttribute(destinationResourceFilterList.get(j),"NID").getValue());
                }

                destinationXpath.add(MediaRoomPolicyHelper.getXPathforSrcDstFilter(destinationUserGroupNidList, destinationResourceGroupNidList));
            }

            for (int i = 0; (i < 30) && (sourceElementList != null) && (sourceFilterElement != null); i++) {
                if (sourceFilterElement[i][0] == null) {
                    break;
                }

                sourceUserFilterList = sourceFilterElement[i][0].elements("TagObjectFilter");
                sourceResourceFilterList = sourceFilterElement[i][1].elements("TagObjectFilter");

                List<String> sourceUserGroupNidList = new ArrayList<String>();
                List<String> sourceResourceGroupNidList = new ArrayList<String>();

                for (int j = 0; (sourceUserFilterList != null) && (j < sourceUserFilterList.size()); j++) {
                    sourceUserGroupNidList.add(MRXMLUtils.getAttribute(sourceUserFilterList.get(j),"NID").getValue());
                }

                for (int j = 0; (sourceResourceFilterList != null) && (j < sourceResourceFilterList.size()); j++) {
                    sourceResourceGroupNidList.add(MRXMLUtils.getAttribute(sourceResourceFilterList.get(j),"NID").getValue());
                }

                sourceXpath.add(MediaRoomPolicyHelper.getXPathforSrcDstFilter(sourceUserGroupNidList, sourceResourceGroupNidList));
            }

            /* //profileFilterList = profileFilterElement.elements("TagObjectFilter");
            
             List<String> profileGroupNidList = new ArrayList();
            
             for (int j = 0; (profileFilterList != null) && (j < profileFilterList.size()); j++) {
                 if ((profileFilterList.get(j) != null) && (profileFilterList.get(j).getAttribute("NID") != null) &&
                         !profileFilterList.get(j).getAttribute("NID").getValue().equals("")) {
                     profileGroupNidList.add(profileFilterList.get(j).getAttribute("NID").getValue());
                 }
             }
            
             profileXpath.add(MediaRoomPolicyHelper.getXPathforObjectFilter(profileGroupNidList, "MediaStreamProfileFilterNID"));
            */

            /*roomFilterList = roomFilterElement.elements("TagObjectFilter");
            List<String> roomGroupNidList= new ArrayList();
            for(int j=0;roomFilterList!=null && j<roomFilterList.size();j++) {
                    if(roomFilterList.get(j)!=null && roomFilterList.get(j).getAttribute("NID")!=null && !roomFilterList.get(j).getAttribute("NID").getValue().equals("")){
                            roomGroupNidList.add(roomFilterList.get(j).getAttribute("NID").getValue());
                    }
            }
            roomXpath.add(MediaRoomPolicyHelper.getXPathforObjectFilter(roomGroupNidList, "MediaRoomObjectFilterNID"));
            */

            /* Element streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath,
                     null,true, serviceName, request, db, log);
            */
            Element roomPolicy = getMatchingRoomPolicyNID(roomPolicyList, destinationList, sourceList,  operatorList, roomXpath, roomGroup, sourceXpath, destinationXpath, serviceName, request, db, log);
            resDoc = generateXML(roomPolicy);
        } catch (MRException ex) {
            ex.printStackTrace();
            throw new MRException(ex.getCode(), ex.getData());
        } catch (Exception ex) {
            ex.printStackTrace();
            throw new MRException(PolicyManagementExceptionCodes.OTHER, ex.getMessage());
        }

        return resDoc;
    }

    @SuppressWarnings("unchecked")
    public static Element getMatchingRoomPolicyNID(List<Element> roomPolicyList, Element destinationList, Element sourceList,  Element operatorList, List<String> roomXpath, Element roomGroup, List<String> sourceXPath, List<String> destinationXpath, String serviceName,
        MRRequest request, MRDB schema, MRLog log) throws Exception {
        Element roomPolicy = null;
       
     
       
        Element roomGroupList = MRXMLUtils.newElement("RoomList");
        Element room = MRXMLUtils.newElement("Room");
        room.add(roomGroup.detach());
        roomGroupList.add(room);

        List<String> roomXpathStringList = new ArrayList<String>();
        roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(roomXpath, "MediaRoomObjectFilterSet", true));
        roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(destinationXpath, "DstFilterSet/DstFilter", true));
        roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(sourceXPath, "SrcFilterSet/SrcFilter", true));

        roomPolicyList = MediaRoomPolicyHelper.matchXPath(roomPolicyList, roomXpathStringList, null);

        List<Element> roomPolicyElementList = PEMImplementation.sortPolicesBasedOnOrder(roomPolicyList);

        for (int i = 0; (roomPolicyElementList != null) && (i < roomPolicyElementList.size()); i++) {
            if (roomPolicyElementList.get(i) != null) {
                if (MediaRoomPolicyHelper.matchRuleKeyTagFilter(roomPolicyElementList.get(i), null, sourceList, destinationList, operatorList, roomGroupList)) {
                    if (roomPolicyElementList.get(i).elementText("allow").equalsIgnoreCase("true")) {
                        roomPolicy = roomPolicyElementList.get(i);

                        break;
                    } else {
                        roomPolicy = null;

                        break;
                    }
                }
            }
        }

        return roomPolicy;
    }

    public static Document generateXML(Element roomPolicy) throws Exception {
        Document responseDoc = MRXMLUtils.stringToDocument("<MatchingRoomPolicy></MatchingRoomPolicy>");

        if (roomPolicy != null) {
            MRXMLUtils.getElementXpath(responseDoc, "//MatchingRoomPolicy").add(roomPolicy.detach());
        }

        return responseDoc;
    }
}
