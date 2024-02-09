package com.ipvs.policyadminservice.handler;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
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


public class GetMediaRoomXPathRequestHandler implements MRRequestHandlerI {
    private static String serviceName = ServiceClient.POLICYADMINSERVICE;

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

   // @SuppressWarnings("unchecked")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document resDoc = MRXMLUtils.getDocument();

        List<?> mediaRoomPolicyList = null;
        List<Element> roomPolicyList = new ArrayList<Element>();

        if (request == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Request"));
        }

        Document xmlDoc = request.getMessage().getDocument();

        if (xmlDoc == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Request Document"));
        }

        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

        if (parentNID == null) {
            return (MRXMLUtils.stringToDocument("<XPathList> </XPathList>"));
        }

        Element dataEL = MRXMLUtils.getElementXpath(xmlDoc, "//GetMediaRoomXPathRequestData");

        if (dataEL == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "GetMediaRoomXPathRequestData"));
        }      
      
        Element operatorList = dataEL.element("OperatorList");

        if (operatorList == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Operator List"));
        }     
        List<?> operatorElementList = operatorList.elements("Operator");

        String action = dataEL.elementText("Action");

        mediaRoomPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorElementList, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, action, "MediaRoom", serviceName,
                request, db, log);

        if ((mediaRoomPolicyList == null) || (mediaRoomPolicyList.size() == 0)) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_MEDIAROOMS, "No Permission available to View MediaRooms");
        }

        for (int i = 0; i < mediaRoomPolicyList.size(); i++) {
            roomPolicyList.add((Element) mediaRoomPolicyList.get(i));
        }

        roomPolicyList = PEMImplementation.sortPolicesBasedOnOrder(roomPolicyList);

        List<String> xPathList = getRoomObjectXpathList(roomPolicyList,  operatorList, ServiceClient.POLICYADMINSERVICE, request, db, log);

        resDoc = generateResDoc(xPathList);

        return resDoc;
    }

    //Generate the Response Document
    private Document generateResDoc(List<String> xPathList) throws MRException, Exception {
        StringBuffer xPath = new StringBuffer();

        Iterator<String> xPathListIterator = xPathList.iterator();
        int count = 0;
        int maxcount = xPathList.size();

        while (xPathListIterator.hasNext()) {
            String xpath = xPathListIterator.next();

            if (xpath.trim().length() != 0) {
                if (xpath.equals(PolicyAdminConstants.ALL_FILTER)) {
                    xPath = new StringBuffer(PolicyAdminConstants.ALL_FILTER);

                    break;
                }

                xPath.append(xPathListIterator.next());

                if (count == (maxcount - 1)) {
                    xPath.append(" or ");
                }
            }
        }

        String xPathStr = "";

        if (xPath.toString().trim().length() != 0) {
            if (xPath.toString().equals(PolicyAdminConstants.ALL_FILTER)) {
                xPathStr = xPath.toString();
            } else {
                xPathStr = "( " + xPath.toString() + ")";
            }
        }

        String xPathStrDoc = "<XPath> " + xPathStr + "</XPath>";

        return MRXMLUtils.stringToDocument(xPathStrDoc);
    }

    //Get the Room Object xPath List from room policies 
    public static List<String> getRoomObjectXpathList(List<Element> roomPolicyListElement,  Element operatorList, String serviceName, MRRequest request,
        MRDB schema, MRLog log) throws Exception {
        Hashtable<String, Element> filterNIDMapping = new Hashtable<String, Element>();
        Hashtable<String, Element> streamFilterNIDMapping = new Hashtable<String, Element>();
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element filterList = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL, null, 1, null, null, request, schema, log);
        List<String> roomPolicyXpath = new ArrayList<String>();

        List<?> filterElement = filterList.elements("TagObjectFilter");

        for (int i = 0; (filterElement != null) && (i < filterElement.size()); i++) {
            filterNIDMapping.put(MRXMLUtils.getAttributeValue(((Element) filterElement.get(i)),"NID"), (Element) filterElement.get(i));
        }

        if (roomPolicyListElement == null) {
            return null;
        }

        for (Iterator<Element> iterRoomPolicy = roomPolicyListElement.iterator(); iterRoomPolicy.hasNext();) {
            Element roomPolicy = iterRoomPolicy.next();

            List<String> roomGroupRuleKeyXpath = new ArrayList<String>();

            if (MediaRoomPolicyHelper.matchRuleKeyTagFilter(roomPolicy, streamFilterNIDMapping,  operatorList, roomGroupRuleKeyXpath)) {
                List<?> roomGroupELList = MRXMLUtils.getListXpath(roomPolicy, "MediaRoomObjectFilterSet/MediaRoomObjectFilterNID");

                if (roomGroupELList.size() != 0) {
                    roomPolicyXpath.add(generateRoomObjectXPath(roomGroupELList, filterNIDMapping, roomGroupRuleKeyXpath));
                }
            }
        }

        return roomPolicyXpath;
    }

    //Generate the room group xPath for given list of media room group object types
    private static String generateRoomObjectXPath(List<?> roomObjectELList, Hashtable<String, Element> filterNIDMapping, List<String> roomObjectRuleKeyXpath)
        throws MRException, Exception {
        Element roomObjectEL = null;
        String resourcefiltername = null;
        Iterator<?> roomObjectELListIT = roomObjectELList.iterator();
        String filterNID = null;
        StringBuffer roomXPath = new StringBuffer();

        int maxCount = roomObjectELList.size();
        int count = 0;
        boolean isAllFilter = false;

        while (roomObjectELListIT.hasNext()) {
            roomObjectEL = (Element) roomObjectELListIT.next();
            filterNID = MRXMLUtils.getValue(roomObjectEL);

            Element filterEL = filterNIDMapping.get(filterNID);

            if (filterEL != null) {
                resourcefiltername = MRXMLUtils.getValueXpath(filterEL, "Name", "");

                if (resourcefiltername.equals(PolicyAdminConstants.ALL_FILTER)) {
                    roomXPath = new StringBuffer();
                    isAllFilter = true;

                    break;
                } else {
                    roomXPath.append(MediaRoomPolicyHelper.generateFilterXpath(filterEL));

                    if (count != (maxCount - 1)) {
                        roomXPath.append(" or ");
                    }
                }
            }

            count++;
        }

        maxCount = roomObjectRuleKeyXpath.size();
        count = 0;

        StringBuffer roomRuleKeyXPath = new StringBuffer();
        Iterator<String> roomObjectRuleKeyIterator = roomObjectRuleKeyXpath.iterator();

        while (roomObjectRuleKeyIterator.hasNext()) {
            roomRuleKeyXPath.append(roomObjectRuleKeyIterator.next());

            if (count == (maxCount - 1)) {
                roomRuleKeyXPath.append(" and ");
            }

            count++;
        }

        String finalXPath = null;

        if (isAllFilter) {
            return PolicyAdminConstants.ALL_FILTER;
        }

        if ((roomRuleKeyXPath.toString().trim().length() != 0) && (roomXPath.toString().trim().length() != 0)) {
            finalXPath = "(" + roomRuleKeyXPath.toString() + "  ) and ( " + roomXPath.toString() + ")";
        } else if ((roomRuleKeyXPath.toString().trim().length() == 0) && (roomXPath.toString().trim().length() != 0)) {
            finalXPath = "( " + roomXPath.toString() + ")";
        } else if ((roomRuleKeyXPath.toString().trim().length() != 0) && (roomXPath.toString().trim().length() == 0)) {
            finalXPath = "( " + roomRuleKeyXPath.toString() + ")";
        }

        return finalXPath;
    }
}
