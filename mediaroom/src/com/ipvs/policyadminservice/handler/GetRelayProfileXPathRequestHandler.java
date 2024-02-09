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
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.policyadminservice.impl.MediaStreamFilterLib;
import com.ipvs.policyadminservice.impl.PEMImplementation;
import com.ipvs.policyadminservice.impl.RelayProfileXPath;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetRelayProfileXPathRequestHandler implements MRRequestHandlerI {
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
    static final int DST_USER_AND_DST_USER = 8;

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

        //List<?> mediaRoomPolicyList = null;
        List<Element> roomPolicyList = new ArrayList<Element>();
       // List<String> roomXpath = new ArrayList<String>();
        List<String> streamXpath = new ArrayList<String>();
       // List<String> roomXpathStringList = new ArrayList<String>();

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
            return MRXMLUtils.stringToDocument("<XPATH></XPATH>");
        }

        Element dataEL = MRXMLUtils.getElementXpath(xmlDoc, "//GetRelayProfileXPathRequestData");

        if (dataEL == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "GetRelayProfileXPathRequestData"));
        }

        // Element roomGroup = dataEL.element("RoomGroup");

        /*Element roomFilterElement = MediaRoomPolicyHelper.findMatchingRoomGroupFilter(roomGroup, serviceName, request , db,  log);
        List<Element> roomFilterList = roomFilterElement.elements("TagObjectFilter");*/
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

       String relayType = MRXMLUtils.getValueXpath(xmlDoc, "//RelayType", "");
       List<String> sourceXpath = new ArrayList<String>();
       List<String> destinationXpath = new ArrayList<String>();

       /*  String action = dataEL.elementText("Action");
           List<?> operatorElementList = operatorList.elements("Operator");
      
    
       // for second relay request no need to check pem directly get the relay
        if (!request.isSuperUser()) {
            mediaRoomPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorElementList, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, action, "MediaRoom", serviceName, request, db, log);

            if ((mediaRoomPolicyList == null) || (mediaRoomPolicyList.size() == 0)) {
                throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_RELAYS, "No Permission available to access Relays");
            }

            for (int i = 0; i < mediaRoomPolicyList.size(); i++) {
                roomPolicyList.add((Element) mediaRoomPolicyList.get(i));
            }

            roomPolicyList = PEMImplementation.sortPolicesBasedOnOrder(roomPolicyList);
        }

   
        if (!request.isSuperUser()) {
            destinationXpath = MediaRoomPolicyHelper.getSrcDestXPath(destinationElementList, true, request, db, log);
            sourceXpath = MediaRoomPolicyHelper.getSrcDestXPath(sourceElementList, true, request, db, log);
            roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(roomXpath, "MediaRoomObjectFilterSet", true));
            roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(sourceXpath, "SrcFilterSet/SrcFilter", true));
            roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(destinationXpath, "DstFilterSet/DstFilter", true));

            roomPolicyList = MediaRoomPolicyHelper.matchXPath(roomPolicyList, roomXpathStringList, null);

            if ((roomPolicyList == null) || (roomPolicyList.size() == 0)) {
                throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_RELAYS, "No Permission available to access Relays");
            }
        }
        
        */

        Element streamFilterElement = null;
        List<Element> streamFilterList = null;

        switch (PolicyAdminConstants.RelayTypeList.valueOf(relayType)) {
        case SourceRelay:
            destinationXpath = MediaRoomPolicyHelper.getSrcDestXPath(destinationElementList, true, request, db, log);              
            sourceXpath = MediaRoomPolicyHelper.getSrcDestXPath(sourceElementList, false, request, db, log);
            streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, true, serviceName, request, db, log);

            break;

        case DestinationRelay:
            destinationXpath = MediaRoomPolicyHelper.getSrcDestXPath(destinationElementList, false, request, db, log);
            sourceXpath = MediaRoomPolicyHelper.getSrcDestXPath(sourceElementList, true, request, db, log);            
            streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath,destinationXpath, true, serviceName, request, db, log);

            break;

        case NoRelay:
            destinationXpath = MediaRoomPolicyHelper.getSrcDestXPath(destinationElementList, false, request, db, log);
            sourceXpath = MediaRoomPolicyHelper.getSrcDestXPath(sourceElementList, false, request, db, log);
            streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, false, serviceName, request, db, log);
            
            break;
            
        case ALLCheck:        
        	 destinationXpath = MediaRoomPolicyHelper.getSrcDestXPath(destinationElementList, true, request, db, log);
             sourceXpath = MediaRoomPolicyHelper.getSrcDestXPath(sourceElementList, true, request, db, log);
             streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, true, serviceName, request, db, log);
             if(streamFilterElement == null) {
            	 streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, false, serviceName, request, db, log);
             }             
             break;             
        default:  
            break;
        }

        if (streamFilterElement != null) {
            streamFilterList = streamFilterElement.elements("MediaStreamFilter");
        }

        List<String> streamFilterNIDList = new ArrayList<String>();

        for (int j = 0; (streamFilterList != null) && (j < streamFilterList.size()); j++) {
            if ((streamFilterList.get(j) != null) && (MRXMLUtils.getAttribute(streamFilterList.get(j),"NID") != null) && !MRXMLUtils.getAttribute(streamFilterList.get(j),"NID").getValue().equals("")) {
                streamFilterNIDList.add(MRXMLUtils.getAttribute(streamFilterList.get(j),"NID").getValue());
            }
        }

        streamXpath.add(MediaRoomPolicyHelper.getXPathforObjectFilter(streamFilterNIDList, "MediaStreamFilterNID"));

        // added for MediaRoomStreamFilterListLookupMode look up flag        
        String streamXPath = MediaRoomPolicyHelper.getXPathforStreamFilter(streamXpath, "MediaRoomStreamFilterSet", true);
        String[] arr = null;

        if ((streamXPath != null) && (streamXPath.length() != 0)) {
            arr = new String[1];
            arr[0] = streamXPath;
        }

        List<Element> matchingPolices = new ArrayList<Element>();

        if (!request.isSuperUser()) {
            for (Iterator<Element> objIt = roomPolicyList.iterator(); objIt.hasNext();) {
                Element roomPolicyEL = objIt.next();
                String mediaRoomStreamFilterListLookupMode = MRXMLUtils.getValueXpath(roomPolicyEL, "MediaRoomStreamFilterSpec/MediaRoomStreamFilterListLookupMode", "");

                if ((mediaRoomStreamFilterListLookupMode != null) && (mediaRoomStreamFilterListLookupMode.trim().length() != 0)) {
                    boolean valid = IpvsDBUtils.validateXpath(roomPolicyEL, arr, request, db, log);

                    if (valid) {
                        matchingPolices.add(roomPolicyEL);
                    }
                } else {
                    matchingPolices.add(roomPolicyEL);
                }
            }

            roomPolicyList = matchingPolices;
        }

        List<RelayProfileXPath> xpathList = null;

        /*  if (!request.isSuperUser()) {
              if (streamFilterElement != null) {
                  xpathList = generateRelayAndProfileFilterXpathList(roomPolicyList, streamFilterElement, destinationList, sourceList, operatorList, roomGroup, serviceName, request, db, log);
              }
          } else {*/
        if (streamFilterElement != null) {
            xpathList = generateRelayProfilefilterxPathList(streamFilterElement, destinationList, sourceList,operatorList, request, db, log);
        }

        // }
        resDoc = MediaStreamFilterLib.generateRelayAndProfileResponseXML(xpathList, relayType);

        return resDoc;
    }

    @SuppressWarnings("unchecked")
    public List<RelayProfileXPath> generateRelayProfilefilterxPathList(Element streamFilterList, Element destinationList, Element sourceList, Element operatorList,MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        List<Element> streamFilterElementList = streamFilterList.elements("MediaStreamFilter");
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element filterList = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL, null, 1, null, null, request, schema, log);
        Hashtable<String, Element> filterNIDMapping = new Hashtable<String, Element>();
        List<Element> filterElement = filterList.elements("TagObjectFilter");

        List<RelayProfileXPath> roomPolicyXpath = new ArrayList<RelayProfileXPath>();

        for (int i = 0; (filterElement != null) && (i < filterElement.size()); i++) {
            filterNIDMapping.put(MRXMLUtils.getAttributeValue(filterElement.get(i),"NID"), filterElement.get(i));
        }

        List<String> profileRuleKeyXpath = null;
        List<String> relayRuleKeyXpath = null;

        if ((streamFilterElementList != null) && (streamFilterElementList.size() != 0)) {
            for (Iterator<Element> objIT = streamFilterElementList.iterator(); objIT.hasNext();) {
                Element StreamFilterEl = objIT.next();
                profileRuleKeyXpath = new ArrayList<String>();
                relayRuleKeyXpath = new ArrayList<String>();

                if (MediaRoomPolicyHelper.matchStreamFilterRuleKey(StreamFilterEl, sourceList, destinationList, null, operatorList, null, MediaRoomPolicyHelper.GET_RELAY_REQUEST, null, null, profileRuleKeyXpath, relayRuleKeyXpath)) {
                    roomPolicyXpath.add(generateRelayAndProfileFilterXpathForRoomPolicy(null, StreamFilterEl, filterNIDMapping, profileRuleKeyXpath, relayRuleKeyXpath));
                }
            }
        }

        /*
        */
        return roomPolicyXpath;
    }

    public static List<RelayProfileXPath> generateRelayAndProfileFilterXpathList(List<Element> roomPolicyListElement, Element streamFilterList, Element destinationList, Element sourceList, Element operatorList, Element roomGroup, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        List<?> streamFilterElementList = streamFilterList.elements("MediaStreamFilter");
        List<RelayProfileXPath> roomPolicyXpath = new ArrayList<RelayProfileXPath>();
        Hashtable<String, Element> filterNIDMapping = new Hashtable<String, Element>();
        Hashtable<String, Element> streamFilterNIDMapping = new Hashtable<String, Element>();
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");

        Element filterList = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL, null, 1, null, null, request, schema, log);

        List<?> filterElement = filterList.elements("TagObjectFilter");
        Element roomGroupList = MRXMLUtils.newElement("RoomList");
        Element room = MRXMLUtils.newElement("Room");
        room.add(roomGroup.detach());
        roomGroupList.add(room);

        for (int i = 0; (filterElement != null) && (i < filterElement.size()); i++) {
            filterNIDMapping.put(MRXMLUtils.getAttributeValue(((Element) filterElement.get(i)),"NID"), (Element) filterElement.get(i));
        }

        for (int i = 0; (streamFilterElementList != null) && (i < streamFilterElementList.size()); i++) {
            streamFilterNIDMapping.put(MRXMLUtils.getAttributeValue(((Element) streamFilterElementList.get(i)),"NID"), (Element) streamFilterElementList.get(i));
        }

        if (roomPolicyListElement == null) {
            return null;
        }

        for (Iterator<Element> iterRoomPolicy = roomPolicyListElement.iterator(); iterRoomPolicy.hasNext();) {
            Element roomPolicy = iterRoomPolicy.next();

            if ((roomPolicy == null)) {
                roomPolicyXpath.add(null);

                continue;
            }

            String mediaRoomStreamFilterListLookupMode = MRXMLUtils.getValueXpath(roomPolicy, "MediaRoomStreamFilterSpec/MediaRoomStreamFilterListLookupMode", "");
            List<?> streamPolicyNIDElement = null;

            if ((mediaRoomStreamFilterListLookupMode != null) && (mediaRoomStreamFilterListLookupMode.trim().length() != 0)) {
                streamPolicyNIDElement = roomPolicy.element("MediaRoomStreamFilterSpec").element("MediaRoomStreamFilterSet").elements("MediaStreamFilterNID");
            } else {
                streamPolicyNIDElement = streamFilterElementList;
            }

            List<String> profileRuleKeyXpath = new ArrayList<String>();
            List<String> relayRuleKeyxPath = new ArrayList<String>();

            if (MediaRoomPolicyHelper.matchRuleKeyTagFilter(roomPolicy, streamFilterNIDMapping, sourceList, destinationList, operatorList, roomGroupList, profileRuleKeyXpath, relayRuleKeyxPath)) {
                for (int j = 0; (streamPolicyNIDElement != null) && (j < streamPolicyNIDElement.size()); j++) {
                    Element streamFilterElement = streamFilterNIDMapping.get( MRXMLUtils.getValue( (Element) streamPolicyNIDElement.get(j)));

                    if (streamFilterElement != null) {
                        roomPolicyXpath.add(generateRelayAndProfileFilterXpathForRoomPolicy(roomPolicy, streamFilterElement, filterNIDMapping, profileRuleKeyXpath, relayRuleKeyxPath));
                    }
                }
            }
        }

        return roomPolicyXpath;
    }

    @SuppressWarnings("unchecked")
    public static RelayProfileXPath generateRelayAndProfileFilterXpathForRoomPolicy(Element roomPolicyElement, Element matchingStreamFilter, Hashtable<String, Element> filterNIDMapping, List<String> profileRuleKeyXpath, List<String> relayRuleKeyXpath)
        throws MRException, Exception {
        List<String> filterXpathList = new ArrayList<String>();
        List<Element> profileNIDList = new ArrayList<Element>();
        List<Element> relayFilterList = new ArrayList<Element>();
        StringBuffer profileXpath = null;
        boolean profileALLCheck = false;
        StringBuffer profileRuleKeyXpathString = null;
        String filterName = "";

        if (matchingStreamFilter == null) {
            return null;
        }

        if ((matchingStreamFilter != null) && (matchingStreamFilter.element("ProfileFilterSet") != null)) {
            profileNIDList.addAll(matchingStreamFilter.element("ProfileFilterSet").elements("ProfileFilter"));
            relayFilterList.addAll(matchingStreamFilter.element("RelayFilterSet").elements());
        }

        for (int i = 0; (profileNIDList != null) && (i < profileNIDList.size()); i++) {
            String profileFilterNID = "";

            if (profileNIDList.get(i) != null) {
                profileFilterNID = profileNIDList.get(i).elementText("MediaStreamProfileFilterNID");
            }

            Element profileFilter = filterNIDMapping.get(profileFilterNID);

            if (profileFilter == null) {
                continue;
            }
            
            filterName = MRXMLUtils.getValue(profileFilter,PolicyAdminConstants.TAG_FILTER_NAME,"");
            if(filterName.equals(PolicyAdminConstants.ALL_FILTER)) {
            	profileALLCheck = true;
            }

            String xPath = MediaRoomPolicyHelper.generateFilterXpath(profileFilter);

            if ((xPath != null) && !xPath.isEmpty()) {
                filterXpathList.add(xPath);
            }
        }

        for (int i = 0; (filterXpathList != null) && (i < filterXpathList.size()); i++) {
            if (profileXpath == null) {
                profileXpath = new StringBuffer();
            }

            profileXpath.append(filterXpathList.get(i));

            if (i < (filterXpathList.size() - 1)) {
                profileXpath.append(" or ");
            }
        }

        for (int i = 0; (profileRuleKeyXpath != null) && (i < profileRuleKeyXpath.size()); i++) {
            if ((profileRuleKeyXpath.get(i) != null) || !"".equals(profileRuleKeyXpath.get(i).trim())) {
                if (profileRuleKeyXpathString == null) {
                    profileRuleKeyXpathString = new StringBuffer();
                } else {
                    profileRuleKeyXpathString.append(" and ");
                }

                profileRuleKeyXpathString.append(profileRuleKeyXpath.get(i));
            }
        }

        RelayProfileXPath relayProfileXPath = new RelayProfileXPath();

        if (((profileXpath != null) && (profileXpath.length() != 0)) && (profileRuleKeyXpathString != null)) {
            relayProfileXPath.setProfilexPath("((" + profileXpath.toString() + ") and (" + profileRuleKeyXpathString.toString() + "))");
        } else if (profileRuleKeyXpathString != null) {
            relayProfileXPath.setProfilexPath("(" + profileRuleKeyXpathString.toString() + ")");
        } else if ((profileXpath != null) && (profileXpath.length() != 0)) {
            relayProfileXPath.setProfilexPath("(" + profileXpath.toString() + ")");
        }  else if ((profileALLCheck)) {
            relayProfileXPath.setProfilexPath("");
        } else {
            relayProfileXPath.setProfilexPath(null);
        }

        String userXpath = null;
        String resourceXpath = null;

        StringBuffer relayRuleKeyXpathString = null;
        Element resourceFilterElement = null;
        Element userFilterElement = null;
        Element relayFilter = null;
        String userFilterNID = null;
        String resourceFilterNID = null;
        boolean resourceFilterALLCheck = false;

        for (Iterator<Element> objIterator = relayFilterList.iterator(); objIterator.hasNext();) {
            relayFilter = objIterator.next();
            userFilterNID = relayFilter.elementText("UserFilterNID");
            resourceFilterNID = relayFilter.elementText("MediaStreamResourceFilterNID");
            userFilterElement = filterNIDMapping.get(userFilterNID);
            resourceFilterElement = filterNIDMapping.get(resourceFilterNID);

            if ((userFilterElement != null) || (resourceFilterElement != null)) {
                userXpath = MediaRoomPolicyHelper.generateFilterXpath(userFilterElement);
                resourceXpath = MediaRoomPolicyHelper.generateFilterXpath(resourceFilterElement);
                
                filterName = MRXMLUtils.getValue(resourceFilterElement,PolicyAdminConstants.TAG_FILTER_NAME,"");
                if(filterName.equals(PolicyAdminConstants.ALL_FILTER)) {
                	resourceFilterALLCheck = true;
                }
            }

            for (int i = 0; (relayRuleKeyXpath != null) && (i < relayRuleKeyXpath.size()); i++) {
                if (i != 0) {
                    relayRuleKeyXpathString.append(" and ");
                } else {
                    relayRuleKeyXpathString = new StringBuffer();
                }

                relayRuleKeyXpathString.append(relayRuleKeyXpath.get(i));
            }
        }

        if ((relayRuleKeyXpathString != null) && (resourceXpath != null) && !relayRuleKeyXpathString.toString().trim().equals("") && !resourceXpath.trim().equals("")) {
            relayProfileXPath.setResourceFilterXPath("(" + resourceXpath + " and ( " + relayRuleKeyXpathString + ") )");
        } else if ((relayRuleKeyXpathString != null) && !relayRuleKeyXpathString.toString().trim().equals("")) {
            relayProfileXPath.setResourceFilterXPath("(" + relayRuleKeyXpathString.toString() + ")");
        } else if ((resourceXpath != null) && !resourceXpath.trim().equals("")) {
            relayProfileXPath.setResourceFilterXPath(resourceXpath);
        } else if (resourceFilterALLCheck) {
            relayProfileXPath.setResourceFilterXPath("");
        } else {
            relayProfileXPath.setResourceFilterXPath(null);
        }

        if ((userXpath != null) && !userXpath.trim().equals("")) {
            relayProfileXPath.setUserFilterXPath(userXpath);
        } else {
            relayProfileXPath.setUserFilterXPath(null);
        }

        return relayProfileXPath;
    }
}
