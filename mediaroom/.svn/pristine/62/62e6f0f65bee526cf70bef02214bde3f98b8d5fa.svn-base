package com.ipvs.policyadminservice.handler;

import java.util.ArrayList;
import java.util.Hashtable;
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
import com.ipvs.policyadminservice.impl.FilterXpath;
import com.ipvs.policyadminservice.impl.PEMImplementation;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetDestinationXPathRequestHandler implements MRRequestHandlerI {
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

    @SuppressWarnings("unchecked")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document resDoc = MRXMLUtils.getDocument();

        List<?> mediaRoomPolicyList = null;
        List<Element> roomPolicyList = new ArrayList<Element>();
     
        // List<String> streamXpath = new ArrayList();
        List<String> roomXpathStringList = new ArrayList<String>();

        if (request == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Request"));
        }

        Document xmlDoc = request.getMessage().getDocument();

        if (xmlDoc == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Request Document"));
        }

        //System.out.println(MRXMLUtils.documentToStringOmitDeclaration(xmlDoc));
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

        if (parentNID == null) {
            return (MRXMLUtils.stringToDocument("<XPathList> </XPathList>"));
        }

        Element dataEL = MRXMLUtils.getElementXpath(xmlDoc, "//GetDestinationXPathRequestData");

        if (dataEL == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "GetDestinationXPathRequestData"));
        }

        
        Element operatorList = dataEL.element("OperatorList");

        if (operatorList == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "Operator List"));
        }
        
        List<?> operatorElementList = operatorList.elements("Operator");
        
        Element sourceList = dataEL.element("SourceList");

        
        List<Element> sourceElementList = null;
        if (sourceList != null) {
        	sourceElementList = sourceList.elements();
        }
        

        //Element roomGroup = dataEL.element("RoomGroup");
        String action = dataEL.elementText("Action");
        mediaRoomPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorElementList, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, action, "MediaRoom", serviceName,
                request, db, log);

        if ((mediaRoomPolicyList == null) || (mediaRoomPolicyList.size() == 0)) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_DESTINATIONS, "No Permission available to access Destinations");
        }

        for (int i = 0; i < mediaRoomPolicyList.size(); i++) {
            roomPolicyList.add((Element) mediaRoomPolicyList.get(i));
        }

        
        if(sourceElementList == null) {
        	List<FilterXpath> roomPolicySrcxPathList = generateMediaRoomDestinationFilterXpathList(roomPolicyList, null, operatorList, null, serviceName, request, db, log);
            resDoc = generateXML(roomPolicySrcxPathList);
            return resDoc;
        }
        
        Element[][] sourceFilterElement = null;

        if (sourceElementList != null) {
            sourceFilterElement = MediaRoomPolicyHelper.findMatchingDestinationGroupFilter_New(sourceElementList, serviceName, request, db, log);
        }

        List<?> sourceUserFilterList = null;
        List<?> sourceResourceFilterList = null;
        List<String> sourceXPath = new ArrayList<String>();

        for (int i = 0; (i < 30) && (sourceElementList != null) && (sourceFilterElement != null); i++) {
            if (sourceFilterElement[i][0] == null) {
                break;
            }

            sourceUserFilterList = sourceFilterElement[i][0].elements("TagObjectFilter");
            sourceResourceFilterList = sourceFilterElement[i][1].elements("TagObjectFilter");

            List<String> sourceUserGroupNidList = new ArrayList<String>();
            List<String> sourceResourceGroupNidList = new ArrayList<String>();

            for (int j = 0; (sourceUserFilterList != null) && (j < sourceUserFilterList.size()); j++) {
                sourceUserGroupNidList.add(MRXMLUtils.getAttribute((Element)sourceUserFilterList.get(j),"NID").getValue());
            }

            for (int j = 0; (sourceResourceFilterList != null) && (j < sourceResourceFilterList.size()); j++) {
                sourceResourceGroupNidList.add(MRXMLUtils.getAttribute((Element)sourceResourceFilterList.get(j),"NID").getValue());
            }

            sourceXPath.add(MediaRoomPolicyHelper.getXPathforSrcDstFilter(sourceUserGroupNidList, sourceResourceGroupNidList));
        }

        roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(sourceXPath, "SrcFilterSet/SrcFilter", true));
        roomPolicyList = MediaRoomPolicyHelper.matchXPath(roomPolicyList, roomXpathStringList, null);

        if ((roomPolicyList == null) || (roomPolicyList.size() == 0)) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_DESTINATIONS, "No Permission available to access Directory");
        }

        List<FilterXpath> roomPolicySrcxPathList = generateMediaRoomDestinationFilterXpathList(roomPolicyList, sourceList, operatorList, null, serviceName, request, db, log);

        resDoc = generateXML(roomPolicySrcxPathList);

        return resDoc;
    }

    @SuppressWarnings("unchecked")
    public static List<FilterXpath> generateMediaRoomDestinationFilterXpathList(List<Element> roomPolicyListElement, Element srcList, Element operatorList, Element roomGroup, String serviceName,
        MRRequest request, MRDB schema, MRLog log) throws Exception {
        List<FilterXpath> roomPolicyXpathList;

        Hashtable<String, Element> filterNIDMapping = new Hashtable<String, Element>();
        Hashtable<String, Element> streamFilterNIDMapping = new Hashtable<String, Element>();
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element filterList = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL, null, 1, null, null, request, schema, log);
        List<Element> filterElement = filterList.elements("TagObjectFilter");
        Element roomGroupList = MRXMLUtils.newElement("RoomList");
        Element room = MRXMLUtils.newElement("Room");

        if (roomGroup != null) {
            room.add(roomGroup.detach());
            roomGroupList.add(room);
        }

        for (int i = 0; (filterElement != null) && (i < filterElement.size()); i++) {
            filterNIDMapping.put(MRXMLUtils.getAttributeValue(filterElement.get(i),"NID"), filterElement.get(i));
        }

        roomPolicyXpathList = new ArrayList<FilterXpath>();

        if (roomPolicyListElement == null) {
            return null;
        }

        for (int i = 0; i < roomPolicyListElement.size(); i++) {
            if (!roomPolicyListElement.get(i).elementText("allow").equalsIgnoreCase("true")) {
                continue;
            }

            List<String> userRuleKeyXpath = new ArrayList<String>();
            List<String> resourceRuleKeyXpath = new ArrayList<String>();
            Element roomPolicy = roomPolicyListElement.get(i);
            Element destFilterSetEL = roomPolicy.element("DstFilterSet");

            if (destFilterSetEL != null) {
                List<Element> destFilterSet = destFilterSetEL.elements("DstFilter");
              
                if (MediaRoomPolicyHelper.matchRuleKeyTagFilter(roomPolicy, streamFilterNIDMapping, srcList, operatorList, roomGroupList, userRuleKeyXpath, resourceRuleKeyXpath, false)) {
                    Element sourceUserFilterElement = null;
                    Element sourceResourceFilterElement = null;
                    for (int j = 0; (destFilterSet != null) && (j < destFilterSet.size()); j++) {
                        Element srcFilter = destFilterSet.get(j);

                        if (srcFilter != null) {
                            String sourceUserFilterNID = srcFilter.elementText("UserFilterNID");
                            String sourceResourceFilterNID = srcFilter.elementText("MediaStreamResourceFilterNID");
                            sourceUserFilterElement = filterNIDMapping.get(sourceUserFilterNID);
                            sourceResourceFilterElement = filterNIDMapping.get(sourceResourceFilterNID);
                        }
                        if ((sourceUserFilterElement != null) || (sourceResourceFilterElement != null)) {                                        
                          roomPolicyXpathList.add(generateSourceFilterXpath(sourceUserFilterElement, sourceResourceFilterElement, userRuleKeyXpath, resourceRuleKeyXpath));
                      }
                    }

                   
                }
            }
        }

        //  Document xpathDoc = generateXML(roomPolicyXpathList);

        //System.out.println(MRXMLUtils.documentToString(xpathDoc));
        return roomPolicyXpathList;
    }

    
    public static FilterXpath generateSourceFilterXpath(Element sourceUserFilter, Element sourceResourceFilter, List<String> userRuleKeyXpath, List<String> resourceRuleKeyXpath)  throws MRException,Exception{
        FilterXpath filterXpath = new FilterXpath();

        //Added by padmaja _ALL_ check -begin 
        String userXpath = null;
        String resourceXpath = null;
        if(sourceUserFilter == null || sourceResourceFilter == null) {
        	return null;
        }
        String userfilterName = sourceUserFilter.elementText("Name");
        String resourcefiltername = sourceResourceFilter.elementText("Name");

        // System.out.println(userfilterName);
        //  System.out.println(resourcefiltername);
        if (userfilterName.equals(PolicyAdminConstants.ALL_FILTER) && resourcefiltername.equals(PolicyAdminConstants.ALL_FILTER)) {
            userXpath = "";
            resourceXpath = "";
        } else if (userfilterName.equals(PolicyAdminConstants.ALL_FILTER)) {
            userXpath = "";
            resourceXpath = MediaRoomPolicyHelper.generateFilterXpath(sourceResourceFilter);
        } else if (resourcefiltername.equals(PolicyAdminConstants.ALL_FILTER)) {
            userXpath = MediaRoomPolicyHelper.generateFilterXpath(sourceUserFilter);
            resourceXpath = "";
        } else {
            userXpath = MediaRoomPolicyHelper.generateFilterXpath(sourceUserFilter);
            resourceXpath = MediaRoomPolicyHelper.generateFilterXpath(sourceResourceFilter);
        }

        //Added by padmaja _ALL_ check -END 
        StringBuffer userRuleKeyXpathString = null;
        StringBuffer resourceRuleKeyXpathString = null;
        if(userRuleKeyXpath == null || resourceRuleKeyXpath == null) {
        	return null;
        }
        for (int i = 0; i < userRuleKeyXpath.size(); i++) {
            if (i != 0) {
                userRuleKeyXpathString.append(" and ");
            } else {
                userRuleKeyXpathString = new StringBuffer();
            }

            userRuleKeyXpathString.append(userRuleKeyXpath.get(i));
        }

        for (int i = 0; i < resourceRuleKeyXpath.size(); i++) {
            if (i != 0) {
                resourceRuleKeyXpathString.append(" and ");
            } else {
                resourceRuleKeyXpathString = new StringBuffer();
            }

            resourceRuleKeyXpathString.append(resourceRuleKeyXpath.get(i));
        }

        if ((resourceRuleKeyXpathString != null) && (resourceXpath != null) && !resourceRuleKeyXpathString.toString().trim().equals("") && !resourceXpath.trim().equals("")) {
            filterXpath.setResourceFilter("(" + resourceXpath + " and ( " + resourceRuleKeyXpathString + ") )");
        } else if ((resourceRuleKeyXpathString != null) && !resourceRuleKeyXpathString.toString().trim().equals("")) {
            filterXpath.setResourceFilter("(" + resourceRuleKeyXpathString.toString() + ")");
        } else if ((resourceXpath != null) && !resourceXpath.trim().equals("")) {
            filterXpath.setResourceFilter(resourceXpath);
        } else {
            filterXpath.setResourceFilter(null);
        }

        if ((userRuleKeyXpathString != null) && (userXpath != null) && !userRuleKeyXpathString.toString().trim().equals("") && !userXpath.trim().equals("")) {
            filterXpath.setUserFilter("(" + userXpath + " and ( " + userRuleKeyXpathString + ") )");
        } else if ((userRuleKeyXpathString != null) && !userRuleKeyXpathString.toString().trim().equals("")) {
            filterXpath.setUserFilter("(" + userRuleKeyXpathString.toString() + ")");
        } else if ((userXpath != null) && !userXpath.trim().equals("")) {
            filterXpath.setUserFilter(userXpath);
        } else {
            filterXpath.setUserFilter(null);
        }

        return filterXpath;
    }

    public static String generateSourceXpath(List<String> xpathList, List<Boolean> isAllowList) {
        StringBuffer xpath = new StringBuffer();

        xpath.append("(");

        for (int i = 0; i < xpathList.size(); i++) {
            if (i != 0) {
                xpath.append(" or ");
            }

            if (isAllowList.get(i).booleanValue()) {
            }
        }

        xpath.append(")");

        return xpath.toString();
    }

    public static Document generateXML(List<FilterXpath> filterXpathList) throws Exception {
        StringBuffer xpathList = new StringBuffer("<XPathList>");

        for (int i = 0; (filterXpathList != null) && (i < filterXpathList.size()); i++) {
            if (filterXpathList.get(i) != null) {
                StringBuffer xpath = new StringBuffer("<XPath>");
                xpath.append("<UserXPath>");

                if (filterXpathList.get(i).getUserFilter() == null) {
                    xpath.append("");
                } else {
                    xpath.append(filterXpathList.get(i).getUserFilter());
                }

                xpath.append("</UserXPath>");
                xpath.append("<MediaStreamResourceXPath>");

                if (filterXpathList.get(i).getResourceFilter() == null) {
                    xpath.append("");
                } else {
                    xpath.append(filterXpathList.get(i).getResourceFilter());
                }

                xpath.append("</MediaStreamResourceXPath>");
                xpath.append("</XPath>");
                xpathList.append(xpath);
            }
        }

        xpathList.append("</XPathList>");

        //System.out.println(xpathList.toString());
        Document responseDoc = MRXMLUtils.stringToDocument(xpathList.toString());

        return responseDoc;
    }
}
