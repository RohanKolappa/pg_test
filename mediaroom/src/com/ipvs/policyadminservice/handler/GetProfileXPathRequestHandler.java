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
import com.ipvs.policyadminservice.impl.PEMImplementation;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetProfileXPathRequestHandler implements MRRequestHandlerI {
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

        //try {
        List<?> mediaRoomPolicyList = null;
        List<Element> roomPolicyList = new ArrayList<Element>();

        //List<String> roomXpath = new ArrayList();
        List<String> streamXpath = new ArrayList<String>();
        List<String> roomXpathStringList = new ArrayList<String>();

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

        @SuppressWarnings("unused")
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE + "List");
        Element dataEL = MRXMLUtils.getElementXpath(xmlDoc, "//GetProfileXPathRequestData");

        if (dataEL == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "GetProfileXPathRequestData"));
        }

        Element roomGroup = dataEL.element("RoomGroup");

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
        List<?> operatorElementList = operatorList.elements("Operator");

        String action = dataEL.elementText("Action");
        mediaRoomPolicyList = PEMImplementation.getMatchingPoliciesListBasedOnOperatorAndRole(operatorElementList, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, action, "MediaRoom", serviceName,
                request, db, log);

        //System.out.println(mediaRoomPolicyList.size());
        if ((mediaRoomPolicyList == null) || (mediaRoomPolicyList.size() == 0)) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_STREAM_PROFILE, "No Permission available to access Profile");

            //return (MRXMLUtils.stringToDocument("<XPathList> </XPathList>"));
        }

        for (int i = 0; i < mediaRoomPolicyList.size(); i++) {
            roomPolicyList.add((Element) mediaRoomPolicyList.get(i));
        }

        //System.out.println(roomPolicyList.size());
        roomPolicyList = PEMImplementation.sortPolicesBasedOnOrder(roomPolicyList);

        //System.out.println(roomPolicyList.size());
        Element[][] destinationFilterElement = MediaRoomPolicyHelper.findMatchingDestinationGroupFilter_New(destinationElementList, serviceName, true, request, db, log);
        Element[][] sourceFilterElement = MediaRoomPolicyHelper.findMatchingDestinationGroupFilter_New(sourceElementList, serviceName, true, request, db, log);
        List<?> destinationUserFilterList = null;
        List<?> destinationResourceFilterList = null;
        List<?> sourceUserFilterList = null;
        List<?> sourceResourceFilterList = null;

        List<String> sourceXpath = new ArrayList<String>();
        List<String> destinationXpath = new ArrayList<String>();

        for (int i = 0; (i < 30) && (destinationElementList != null); i++) {
            if (destinationFilterElement[i][0] == null) {
                break;
            }

            destinationUserFilterList = destinationFilterElement[i][0].elements("TagObjectFilter");
            destinationResourceFilterList = destinationFilterElement[i][1].elements("TagObjectFilter");

            List<String> destinationUserGroupNidList = new ArrayList<String>();
            List<String> destinationResourceGroupNidList = new ArrayList<String>();

            for (int j = 0; (destinationUserFilterList != null) && (j < destinationUserFilterList.size()); j++) {
                destinationUserGroupNidList.add(MRXMLUtils.getAttribute((Element)destinationUserFilterList.get(j),"NID").getValue());
            }

            for (int j = 0; (destinationResourceFilterList != null) && (j < destinationResourceFilterList.size()); j++) {
                destinationResourceGroupNidList.add(MRXMLUtils.getAttribute((Element)destinationResourceFilterList.get(j),"NID").getValue());
            }

            String tempDestinationXpath = MediaRoomPolicyHelper.getXPathforSrcDstFilter(destinationUserGroupNidList, destinationResourceGroupNidList);

            if (tempDestinationXpath != null) {
                destinationXpath.add(tempDestinationXpath);
            }
        }

        for (int i = 0; (i < 30) && (sourceElementList != null); i++) {
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

            String tempSourceXpath = MediaRoomPolicyHelper.getXPathforSrcDstFilter(sourceUserGroupNidList, sourceResourceGroupNidList);

            if (tempSourceXpath != null) {
                sourceXpath.add(tempSourceXpath);
            }
        }

        //TBD :Need to remove after roomgroup get as input
        /*roomFilterList = roomFilterElement.elements("TagObjectFilter");
        List<String> roomGroupNidList= new ArrayList();
        for(int j=0;roomFilterList!=null && j<roomFilterList.size();j++) {
                if(roomFilterList.get(j)!=null && roomFilterList.get(j).getAttribute("NID")!=null && !roomFilterList.get(j).getAttribute("NID").getValue().equals("")){
                        roomGroupNidList.add(roomFilterList.get(j).getAttribute("NID").getValue());
                }
        }*/

        //roomXpath.add(MediaRoomPolicyHelper.getXPathforObjectFilter(roomGroupNidList, "MediaRoomObjectFilterNID"));
        Element streamFilterElement = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath,false,true, serviceName, request, db, log);
        List<?> streamFilterList = null;

        if (streamFilterElement == null) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_STREAM_PROFILE, "No Permission available to access Profile");
        }

        if (streamFilterElement != null) {
            streamFilterList = streamFilterElement.elements("MediaStreamFilter");
        }

        List<String> streamFilterNIDList = new ArrayList<String>();

        for (int j = 0; (streamFilterList != null) && (j < streamFilterList.size()); j++) {
            if ((streamFilterList.get(j) != null) && (MRXMLUtils.getAttribute((Element)streamFilterList.get(j),"NID") != null) && !MRXMLUtils.getAttribute((Element)streamFilterList.get(j),"NID").getValue().equals("")) {
                streamFilterNIDList.add(MRXMLUtils.getAttribute((Element)streamFilterList.get(j),"NID").getValue());
            }
        }

        String tempStreamXpath = MediaRoomPolicyHelper.getXPathforObjectFilter(streamFilterNIDList, "MediaStreamFilterNID");
        
        if(streamXpath == null) {
        	 throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_STREAM_PROFILE, "No Permission available to access Profile");
        }
        
        if (streamXpath != null) {
            streamXpath.add(tempStreamXpath);
        } /* else {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_STREAM_PROFILE, "No Permission available to access Profile");
        } */

        
       // roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(streamXpath, "MediaRoomStreamFilterSet",true));        
        roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(sourceXpath, "SrcFilterSet/SrcFilter",true));        
        roomXpathStringList.add(MediaRoomPolicyHelper.getXPathforStreamFilter(destinationXpath, "DstFilterSet/DstFilter",true));
        
        roomPolicyList = MediaRoomPolicyHelper.matchXPath(roomPolicyList, roomXpathStringList, null);

        if ((roomPolicyList == null) || (roomPolicyList.size() == 0)) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_STREAM_PROFILE, "No Permission available to access Profile");
        }

        // added for MediaRoomStreamFilterListLookupMode look up flag        
        roomXpathStringList = new ArrayList<String>();

        String streamXPath = MediaRoomPolicyHelper.getXPathforStreamFilter(streamXpath, "MediaRoomStreamFilterSet",true);      
        String[] arr = { streamXPath };
        List<Element> matchingPolices = new ArrayList<Element>();

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

        if (matchingPolices.size() == 0) {
            throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_ACCESS_STREAM_PROFILE, "No Permission available to access Profile");
        }

        roomPolicyList = matchingPolices;

        //System.out.println(roomPolicyList.size());
        List<String> xpathList = generateProfileFilterXpathList(roomPolicyList, streamFilterElement, destinationList, sourceList, operatorList, roomGroup, serviceName, request, db, log);
        String xpath = generateProfileFilterXpath(roomPolicyList, xpathList);
        //System.out.println(xpath);
        resDoc = generateXML(xpath);

        /*} catch (MRException ex) {
                ex.printStackTrace();
                throw new MRException(ex.getCode(), ex.getData());
        } catch (Exception ex) {
                ex.printStackTrace();
                throw new MRException(PolicyManagementExceptionCodes.OTHER,
                                ex.getMessage());
        }*/
        return resDoc;
    }

    @SuppressWarnings("unchecked")
    public static List<String> generateProfileFilterXpathList(List<Element> roomPolicyListElement, Element streamFilterList, Element destinationList, Element sourceList, Element operatorList,
        Element roomGroup, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        List<Element> matchingStreamFilter = null;
        List<Element> streamFilterElementList = streamFilterList.elements("MediaStreamFilter");
        List<String> roomPolicyXpath = new ArrayList<String>();
        //@SuppressWarnings("unused")
        //StringBuffer xpath = new StringBuffer();
        Hashtable<String, Element> filterNIDMapping = new Hashtable<String, Element>();
        Hashtable<String, Element> streamFilterNIDMapping = new Hashtable<String, Element>();
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element filterList = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL, null, 1, null, null, request, schema, log);

        //Element filterList = MRXMLDBUtils.findChildElements(parentNID,responseEL, null, 1, null, null, request, schema, log);
        List<Element> filterElement = filterList.elements("TagObjectFilter");
        Element roomGroupList = MRXMLUtils.newElement("RoomList");
        Element room = MRXMLUtils.newElement("Room");
        room.add(roomGroup.detach());
        roomGroupList.add(room);

        for (int i = 0; (filterElement != null) && (i < filterElement.size()); i++) {
            filterNIDMapping.put(MRXMLUtils.getAttributeValue(filterElement.get(i),"NID"), filterElement.get(i));
        }

        for (int i = 0; (streamFilterElementList != null) && (i < streamFilterElementList.size()); i++) {
            streamFilterNIDMapping.put(MRXMLUtils.getAttributeValue(streamFilterElementList.get(i),"NID"), streamFilterElementList.get(i));
        }

        if (roomPolicyListElement == null) {
            return null;
        }

        //System.out.println(roomPolicyListElement.size());
        for (Iterator<Element> iterRoomPolicy = roomPolicyListElement.iterator(); iterRoomPolicy.hasNext();) {
            Element roomPolicy = iterRoomPolicy.next();

            if ((roomPolicy == null)) {
                roomPolicyXpath.add(null);

                continue;
            }

            matchingStreamFilter = new ArrayList<Element>();

            String mediaRoomStreamFilterListLookupMode = MRXMLUtils.getValueXpath(roomPolicy, "MediaRoomStreamFilterSpec/MediaRoomStreamFilterListLookupMode", "");
            List<Element> streamPolicyNIDElement = null;

            if ((mediaRoomStreamFilterListLookupMode != null) && (mediaRoomStreamFilterListLookupMode.trim().length() != 0)) {
                streamPolicyNIDElement = roomPolicy.element("MediaRoomStreamFilterSpec").element("MediaRoomStreamFilterSet").elements("MediaStreamFilterNID");
            } else {
                streamPolicyNIDElement = streamFilterElementList;
            }

            List<String> profileRuleKeyXpath = new ArrayList<String>();

            if (MediaRoomPolicyHelper.matchRuleKeyTagFilter(roomPolicy, streamFilterNIDMapping, sourceList, destinationList, operatorList, roomGroupList, profileRuleKeyXpath)) {
                for (int j = 0; (streamPolicyNIDElement != null) && (j < streamPolicyNIDElement.size()); j++) {
                    Element streamFilterElement = streamFilterNIDMapping.get(MRXMLUtils.getValue(streamPolicyNIDElement.get(j)));

                    if (streamFilterElement != null) {
                        matchingStreamFilter.add(streamFilterElement);
                    }
                }
            }

            roomPolicyXpath.add(generateProfileFilterXpathForRoomPolicy(roomPolicy, matchingStreamFilter, filterNIDMapping, profileRuleKeyXpath));
        }

        return roomPolicyXpath;
    }

    public static String generateProfileFilterXpath(List<Element> roomPolicyList, List<String> xpath) {
        StringBuffer allowList = null;
        StringBuffer denyList = null;
        StringBuffer mergedList = null;

        for (int i = 0; (roomPolicyList != null) && (i < roomPolicyList.size()); i++) {
            if (roomPolicyList.get(i).elementText("allow").equals("true") && ((xpath.get(i) != null) && (xpath.get(i).length() != 0))) {
                if (allowList == null) {
                    allowList = new StringBuffer();
                } else {
                    allowList.append(" or ");
                }

                allowList.append(xpath.get(i));
            } else if (roomPolicyList.get(i).elementText("allow").equals("false") && ((xpath.get(i) != null) && (xpath.get(i).length() != 0))) {
                if (denyList != null) {
                    if (allowList != null) {
                        if (mergedList == null) {
                            mergedList = new StringBuffer();
                        } else {
                            mergedList.append(" or ");
                        }

                        mergedList.append("( " + "( " + allowList.toString() + ") " + " and " + " not ( " + denyList.toString() + " ) " + ")");
                        allowList = null;
                    }

                    denyList.append(" or ");
                } else {
                    denyList = new StringBuffer();
                }

                denyList.append(xpath.get(i));
            }
        }

        if (allowList != null) {
            if (mergedList == null) {
                mergedList = new StringBuffer();
            } else {
                mergedList.append(" or ");
            }

            if (denyList == null) {
                mergedList.append("( " + "( " + allowList.toString() + ") " + ")");
            } else {
                mergedList.append("( " + "( " + allowList.toString() + ") " + " and " + " not ( " + denyList.toString() + " ) " + ")");
            }

            allowList = null;
        }

        if (mergedList != null) {
            return mergedList.toString();
        } else {
            return "";
        }
    }

    @SuppressWarnings("unchecked")
    public static String generateProfileFilterXpathForRoomPolicy(Element roomPolicyElement, List<Element> matchingStreamFilter, Hashtable<String, Element> filterNIDMapping,
        List<String> profileRuleKeyXpath) throws MRException,Exception{
    	List<String> filterXpathList = new ArrayList<String>();
    	List<Element> profileNIDList = new ArrayList<Element>();
        StringBuffer profileXpath = null;
        StringBuffer profileRuleKeyXpathString = null;

        if (matchingStreamFilter == null) {
            return null;
        }

        for (int i = 0; i < matchingStreamFilter.size(); i++) {
            Element streamFilter = matchingStreamFilter.get(i);

            if ((streamFilter != null) && (streamFilter.element("ProfileFilterSet") != null)) {
                profileNIDList.addAll(streamFilter.element("ProfileFilterSet").elements("ProfileFilter"));
            }
        }

        boolean allFilter = false;

        for (int i = 0; (profileNIDList != null) && (i < profileNIDList.size()); i++) {
            String profileFilterNID = "";

            if (profileNIDList.get(i) != null) {
                profileFilterNID = profileNIDList.get(i).elementText("MediaStreamProfileFilterNID");
            }

            Element profileFilter = filterNIDMapping.get(profileFilterNID);
            String name = profileFilter.elementText("Name");

            if (name.equals(PolicyAdminConstants.ALL_FILTER)) {
                if ((profileRuleKeyXpath == null) || (profileRuleKeyXpath.size() == 0)) {
                    //TBR: remove retrun statement when we are considering deny conditions for ALL filter
                    return "";
                }

                allFilter = true;
                profileXpath = new StringBuffer("");

                break;
            }


            String xPath = MediaRoomPolicyHelper.generateFilterXpath(profileFilter);

            if ((xPath != null) && !xPath.isEmpty()) {
                filterXpathList.add(xPath);
            }
        }

        //TBR: remove if condition when we are considering deny conditions for ALL filter
        if (!allFilter) {
            for (int i = 0; (filterXpathList != null) && (i < filterXpathList.size()); i++) {
            	if (profileXpath == null) {
                    profileXpath = new StringBuffer();
                }

                profileXpath.append(filterXpathList.get(i));

                if (i < (filterXpathList.size() - 1)) {
                	 profileXpath.append(" or ");
                }                    
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

        //System.out.println("prfilexpath----------->" + profileXpath);
        if (((profileXpath != null) && (profileXpath.length() != 0)) && (profileRuleKeyXpathString != null)) {
            return "((" + profileXpath.toString() + ") and (" + profileRuleKeyXpathString.toString() + "))";
        } else if (profileRuleKeyXpathString != null) {
            return "(" + profileRuleKeyXpathString.toString() + ")";
        } else if ((profileXpath != null) && (profileXpath.length() != 0)) {
            return "(" + profileXpath.toString() + ")";
        } else {
            return null;
        }
    }

    public static Document generateXML(String xpath) throws Exception {
        Document responseDoc = MRXMLUtils.stringToDocument("<XPATH>" + xpath + "</XPATH>");

        return responseDoc;
    }
}
