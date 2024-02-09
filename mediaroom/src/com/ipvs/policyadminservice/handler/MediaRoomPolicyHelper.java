package com.ipvs.policyadminservice.handler;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Node;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.impl.PEMImplementation;
import com.ipvs.policyadminservice.utils.MediaRoomRuleKey;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;
import com.ipvs.utils.Utils;


public class MediaRoomPolicyHelper {
    public static final int DST_USER_AND_DST_RESOURCE = 1;
    public static final int OPERATOR_USER_AND_DST_USER = 2;
    public static final int OPERATOR_USER_AND_DST_RESOURCE = 3;
    public static final int DST_USER_AND_ROOM = 4;
    public static final int DST_RESOURCE_AND_ROOM = 5;
    public static final int SRC_USER_AND_ROOM = 6;
    public static final int SRC_RESOURCE_AND_ROOM = 7;
    public static final int OPERATOR_USER_AND_ROOM = 8;
    public static final int DST_USER_AND_SRC_USER = 9;
    public static final int DST_USER_AND_SRC_RESOURCE = 10;
    public static final int SRC_USER_AND_SRC_RESOURCE = 11;
    public static final int DST_RESOURCE_AND_SRC_USER = 12;
    public static final int DST_RESOURCE_AND_SRC_RESOURCE = 13;
    public static final int OPERATOR_USER_AND_SRC_USER = 14;
    public static final int OPERATOR_USER_AND_SRC_RESOURCE = 15;
    public static final int PROFILE_AND_ROOM = 16;
    public static final int DST_USER_AND_PROFILE = 17;
    public static final int DST_RESOURCE_AND_PROFILE = 18;
    public static final int SRC_USER_AND_PROFILE = 19;
    public static final int SRC_RESOURCE_AND_PROFILE = 20;
    public static final int OPERATOR_USER_AND_PROFILE = 21;

    //Relay Combinations -- added by padmaja
    public static final int SRC_USER_AND_RELAY_RESOURCE = 22;
    public static final int SRC_RESOURCE_AND_RELAY_RESOURCE = 23;
    public static final int DST_USER_AND_RELAY_RESOURCE = 24;
    public static final int DST_RESOURCE_AND_RELAY_RESOURCE = 25;
    public static final int OPERATOR_USER_AND_RELAY_RESOURCE = 26;
    public static final int OTHER = 27;
    public static final int GET_SOURCE_REQUEST = 101;
    public static final int GET_PROFILE_REQUEST = 102;
    public static final int GET_MATCHING_POLICY_REQUEST = 103;
    public static final int GET_DESTINATION_REQUEST = 104;
    public static final int GET_RELAY_REQUEST = 105;
    public static final int GET_MEDIA_ROOM_REQUEST = 106;
    public static final String USER = "User";
    public static final String RESOURCE = "Resource";
    public static final String PROFILE = "Profile";
    private static Hashtable<CaseFilter, Integer> caseHash = null;

    // private static List<Element> streamFilterList = null;
    public static synchronized void init() {
        if (caseHash == null) {
            caseHash = new Hashtable<CaseFilter, Integer>();

            CaseFilter caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(DST_USER_AND_DST_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(SRC_USER_AND_SRC_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER);
            caseHash.put(caseFilter, Integer.valueOf(DST_USER_AND_SRC_USER));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(DST_USER_AND_SRC_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER);
            caseHash.put(caseFilter, Integer.valueOf(DST_RESOURCE_AND_SRC_USER));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(DST_RESOURCE_AND_SRC_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_DST_USER));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_DST_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_SRC_USER));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_SRC_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);
            caseHash.put(caseFilter, Integer.valueOf(DST_USER_AND_PROFILE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);
            caseHash.put(caseFilter, Integer.valueOf(DST_RESOURCE_AND_PROFILE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);
            caseHash.put(caseFilter, Integer.valueOf(SRC_USER_AND_PROFILE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);
            caseHash.put(caseFilter, Integer.valueOf(SRC_RESOURCE_AND_PROFILE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_PROFILE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);
            caseHash.put(caseFilter, Integer.valueOf(PROFILE_AND_ROOM));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);
            caseHash.put(caseFilter, Integer.valueOf(DST_USER_AND_ROOM));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);
            caseHash.put(caseFilter, Integer.valueOf(DST_RESOURCE_AND_ROOM));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);
            caseHash.put(caseFilter, Integer.valueOf(SRC_USER_AND_ROOM));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);
            caseHash.put(caseFilter, Integer.valueOf(SRC_RESOURCE_AND_ROOM));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_ROOM));
            //Relay Combinations - added by padmaja
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_RELAY_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(OPERATOR_USER_AND_RELAY_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_RELAY_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(SRC_USER_AND_RELAY_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_RELAY_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(SRC_RESOURCE_AND_RELAY_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_RELAY_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(DST_USER_AND_RELAY_RESOURCE));
            caseFilter = new CaseFilter(MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_RELAY_RESOURCE);
            caseHash.put(caseFilter, Integer.valueOf(DST_RESOURCE_AND_RELAY_RESOURCE));
        }
    }

    public static int getCase(Element ruleKeyTagFilterElement) {
        String ruleKeyLHRuleKey = ruleKeyTagFilterElement.element("RuleKeyLHTagName").elementText("RuleKey");
        String ruleKeyRHRuleKey = ruleKeyTagFilterElement.element("RuleKeyRHTagName").elementText("RuleKey");
        CaseFilter caseFilter = new CaseFilter(ruleKeyLHRuleKey, ruleKeyRHRuleKey);       
        Integer intCase = caseHash.get(caseFilter);

        if (intCase != null) {
            return caseHash.get(caseFilter).intValue();
        } else {
            return OTHER;
        }
    }

    @SuppressWarnings("unchecked")
    public static boolean matchRuleKeyTagFilterCase(Element ruleKeyFilterElement, int ruleCase, Element sourceList, Element destinationList, Element profileList, Element operatorList, Element roomList)
        throws Exception {
        boolean isMatch = true;
        List<Element> destinationElement = null;

        if (destinationList != null) {
            destinationElement = destinationList.elements();
        }

        List<Element> operatorElement = null;

        if (operatorList != null) {
            operatorElement = operatorList.elements();
        }

        List<Element> sourceElement = null;

        if (sourceList != null) {
            //System.out.println(MRXMLUtils.elementToString(sourceList));
            sourceElement = sourceList.elements();
        }

        List<Element> profileElement = null;

        if (profileList != null) {
            profileElement = profileList.elements();
        }

        List<Element> roomElement = null;

        if (roomList != null) {
            roomElement = roomList.elements();
        }

        switch (ruleCase) {
        case DST_USER_AND_DST_RESOURCE:
            isMatch = matchRuleKey(destinationElement, destinationElement, USER, RESOURCE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE);

            break;

        case DST_USER_AND_SRC_USER:
            isMatch = matchRuleKey(destinationElement, sourceElement, USER, USER, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER);

            break;

        case DST_USER_AND_SRC_RESOURCE:
            isMatch = matchRuleKey(destinationElement, sourceElement, USER, RESOURCE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);

            break;

        case DST_USER_AND_PROFILE:
            isMatch = matchRuleKey(destinationElement, profileElement, USER, PROFILE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);

            break;

        case OPERATOR_USER_AND_DST_USER:
            isMatch = matchRuleKey(operatorElement, destinationElement, USER, USER, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER);

            break;

        case OPERATOR_USER_AND_DST_RESOURCE:
            isMatch = matchRuleKey(operatorElement, destinationElement, USER, RESOURCE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE);

            break;

        case OPERATOR_USER_AND_SRC_USER:
            isMatch = matchRuleKey(operatorElement, sourceElement, USER, USER, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER);

            break;

        case OPERATOR_USER_AND_SRC_RESOURCE:
            isMatch = matchRuleKey(operatorElement, sourceElement, USER, RESOURCE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);

            break;

        case DST_RESOURCE_AND_SRC_USER:
            isMatch = matchRuleKey(destinationElement, sourceElement, RESOURCE, USER, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER);

            break;

        case DST_RESOURCE_AND_SRC_RESOURCE:
            isMatch = matchRuleKey(destinationElement, sourceElement, RESOURCE, RESOURCE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);

            break;

        case SRC_USER_AND_SRC_RESOURCE:
            isMatch = matchRuleKey(sourceElement, sourceElement, USER, RESOURCE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE);

            break;

        case SRC_USER_AND_PROFILE:
            isMatch = matchRuleKey(sourceElement, profileElement, USER, PROFILE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);

            break;

        case SRC_RESOURCE_AND_PROFILE:
            isMatch = matchRuleKey(sourceElement, profileElement, RESOURCE, PROFILE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);

            break;

        case DST_RESOURCE_AND_PROFILE:
            isMatch = matchRuleKey(destinationElement, profileElement, RESOURCE, PROFILE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);

            break;

        case OPERATOR_USER_AND_PROFILE:
            isMatch = matchRuleKey(operatorElement, sourceElement, USER, PROFILE, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_PROFILE);

            break;

        case DST_USER_AND_ROOM:
            isMatch = matchRuleKey(destinationElement, roomElement, USER, "RoomGroup", ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);

            break;

        case DST_RESOURCE_AND_ROOM:
            isMatch = matchRuleKey(destinationElement, profileElement, RESOURCE, "RoomGroup", ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);

            break;

        case SRC_USER_AND_ROOM:
            isMatch = matchRuleKey(sourceElement, roomElement, USER, "RoomGroup", ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);

            break;

        case SRC_RESOURCE_AND_ROOM:
            isMatch = matchRuleKey(sourceElement, roomElement, RESOURCE, "RoomGroup", ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);

            break;

        case OPERATOR_USER_AND_ROOM:
            isMatch = matchRuleKey(operatorElement, roomElement, USER, "RoomGroup", ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);

            break;

        case PROFILE_AND_ROOM:
            isMatch = matchRuleKey(profileElement, roomElement, PROFILE, "RoomGroup", ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_TARGET_MEDIAROOM);

            break;

        default:
            isMatch = true;

            break;
        }

        return isMatch;
    }

    //Get Relay List Rule Key check -- added by Padmaja
    @SuppressWarnings("unchecked")
    public static boolean matchRuleKeyTagFilterCase(Element ruleKeyFilterElement, int ruleCase, Element sourceList, Element destinationList, Element operatorList, Element roomList, List<String> profileRuleKeyXpath, boolean isRelay)
        throws Exception {
        boolean isMatch = true;

        List<Element> destinationElement = null;

        if (destinationList != null) {
            destinationElement = destinationList.elements();
        }

        List<Element> operatorElement = null;

        if (operatorList != null) {
            operatorElement = operatorList.elements();
        }

        List<Element> sourceElement = null;

        if (sourceList != null) {
            sourceElement = sourceList.elements();
        }

        @SuppressWarnings("unused")
        List<Element> roomElement = null;

        /*if (roomList != null) {
            roomElement = roomList.elements();
        }*/
        String ruleKeyXpath = null;

        switch (ruleCase) {
        case SRC_USER_AND_RELAY_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, USER);

            break;

        case DST_USER_AND_RELAY_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, USER);

            break;

        case SRC_RESOURCE_AND_RELAY_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, RESOURCE);

            break;

        case DST_RESOURCE_AND_RELAY_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, RESOURCE);

            break;

        case OPERATOR_USER_AND_RELAY_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, USER);

            break;

        default:

            if (ruleCase <= 15) {
                return matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, null, operatorList, roomList);
            }

            return true;
        }

        if (ruleKeyXpath != null) {
            profileRuleKeyXpath.add(ruleKeyXpath);
        } else {
            isMatch = false;
        }

        return isMatch;
    }

    //Get media room list rule key check
    @SuppressWarnings("unchecked")
    public static boolean matchRuleKeyTagFilterCase(Element ruleKeyFilterElement, int ruleCase, Element operatorList, List<String> roomruleKeyXPath)
        throws Exception {
        boolean isMatch = true;

        List<Element> operatorElement = null;

        if (operatorList != null) {
            operatorElement = operatorList.elements();
        }

        String ruleKeyXpath = null;

        switch (ruleCase) {
        case OPERATOR_USER_AND_ROOM:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, USER);

            break;

        default:
            return true;
        }

        if (ruleKeyXpath != null) {
            roomruleKeyXPath.add(ruleKeyXpath);
        } else {
            isMatch = false;
        }

        return isMatch;
    }

    //2
    @SuppressWarnings("unchecked")
    public static boolean matchRuleKeyTagFilterCase(Element ruleKeyFilterElement, int ruleCase, Element sourceList, Element destinationList, Element operatorList, Element roomList, List<String> profileRuleKeyXpath)
        throws Exception {
        boolean isMatch = true;

        List<Element> destinationElement = null;

        if (destinationList != null) {
            destinationElement = destinationList.elements();
        }

        List<Element> operatorElement = null;

        if (operatorList != null) {
            operatorElement = operatorList.elements();
        }

        List<Element> sourceElement = null;

        if (sourceList != null) {
            sourceElement = sourceList.elements();
        }

        @SuppressWarnings("unused")
        List<Element> roomElement = null;

        /*if (roomList != null) {
            roomElement = roomList.elements();
        }*/
        String ruleKeyXpath = null;

        switch (ruleCase) {
        case SRC_USER_AND_PROFILE:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, USER);

            break;

        case DST_USER_AND_PROFILE:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, USER);

            break;

        case SRC_RESOURCE_AND_PROFILE:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, RESOURCE);

            break;

        case DST_RESOURCE_AND_PROFILE:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, RESOURCE);

            break;

        case OPERATOR_USER_AND_PROFILE:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_CONFCREATE_USER, USER);

            break;

        default:

            if (ruleCase <= 15) {
                return matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, null, operatorList, roomList);
            }

            return true;
        }

        if (ruleKeyXpath != null) {
            profileRuleKeyXpath.add(ruleKeyXpath);
        } else {
            isMatch = false;
        }

        return isMatch;
    }

    //case 4 get destination list 
    @SuppressWarnings("unchecked")
    public static boolean matchRuleKeyTagFilterCase(Element ruleKeyFilterElement, int ruleCase, Element sourceList, Element operatorList, Element roomList, List<String> userRuleKeyXpath, List<String> resourceRuleKeyXpath, boolean sourceFlag)
        throws Exception {
        boolean isMatch = true;
        List<Element> sourceElement = null;

        if (sourceList != null) {
            sourceElement = sourceList.elements();
        }

        List<Element> operatorElement = null;

        if (operatorList != null) {
            operatorElement = operatorList.elements();
        }

        @SuppressWarnings("unused")
        List<Element> roomElement = null;

        /*if (roomList != null) {
            roomElement = roomList.elements();
        }*/
        String ruleKeyXpath = null;

        switch (ruleCase) {
        case DST_USER_AND_SRC_USER:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, USER);

            if (ruleKeyXpath != null) {
                userRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case DST_USER_AND_SRC_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, USER);

            if (ruleKeyXpath != null) {
                resourceRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case DST_RESOURCE_AND_SRC_USER:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, RESOURCE);

            if (ruleKeyXpath != null) {
                userRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case DST_RESOURCE_AND_SRC_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(sourceElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, RESOURCE);

            if (ruleKeyXpath != null) {
                resourceRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case OPERATOR_USER_AND_DST_USER:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_USER, USER);

            if (ruleKeyXpath != null) {
                userRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case OPERATOR_USER_AND_DST_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_DST_RESOURCE, USER);

            if (ruleKeyXpath != null) {
                resourceRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        default:

            if (ruleCase <= 15) {
                return matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, null, null, operatorList, roomList);
            }

            return true;
        }

        if (ruleKeyXpath == null) {
            isMatch = false;
        }

        return isMatch;
    }

    //	case 3
    @SuppressWarnings("unchecked")
    public static boolean matchRuleKeyTagFilterCase(Element ruleKeyFilterElement, int ruleCase, Element destinationList, Element operatorList, Element roomList, List<String> userRuleKeyXpath, List<String> resourceRuleKeyXpath)
        throws Exception {
        boolean isMatch = true;
        List<Element> destinationElement = null;

        if (destinationList != null) {
            destinationElement = destinationList.elements();
        }

        List<Element> operatorElement = null;

        if (operatorList != null) {
            operatorElement = operatorList.elements();
        }

        @SuppressWarnings("unused")
        List<Element> roomElement = null;

        /*if (roomList != null) {
            roomElement = roomList.elements();
        }*/
        String ruleKeyXpath = null;

        switch (ruleCase) {
        case DST_USER_AND_SRC_USER:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, USER);

            if (ruleKeyXpath != null) {
                userRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case DST_USER_AND_SRC_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, USER);

            if (ruleKeyXpath != null) {
                resourceRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case DST_RESOURCE_AND_SRC_USER:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, RESOURCE);

            if (ruleKeyXpath != null) {
                userRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case DST_RESOURCE_AND_SRC_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(destinationElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, RESOURCE);

            if (ruleKeyXpath != null) {
                resourceRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case OPERATOR_USER_AND_SRC_USER:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_USER, USER);

            if (ruleKeyXpath != null) {
                userRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        case OPERATOR_USER_AND_SRC_RESOURCE:
            ruleKeyXpath = generateRuleKeyXpath_New(operatorElement, ruleKeyFilterElement, MediaRoomRuleKey.MEDIAROOM_RULE_STREAM_SRC_RESOURCE, USER);

            if (ruleKeyXpath != null) {
                resourceRuleKeyXpath.add(ruleKeyXpath);
            }

            break;

        default:

            if (ruleCase <= 8) {
                return matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, null, destinationList, null, operatorList, roomList);
            }

            return true;
        }

        if (ruleKeyXpath == null) {
            isMatch = false;
        }

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element sourceList, Element destinationList, Element profileList, Element operatorList, Element roomList, int requestType, List<String> userRuleKeyXpath,
        List<String> resourceRuleKeyXpath, List<String> profileRuleKeyXpath)
        throws Exception {
        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, sourceList, destinationList, profileList, operatorList, roomList, requestType, userRuleKeyXpath, resourceRuleKeyXpath, profileRuleKeyXpath, null);

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element sourceList, Element destinationList, Element profileList, Element operatorList, Element roomList, int requestType, List<String> userRuleKeyXpath,
        List<String> resourceRuleKeyXpath, List<String> profileRuleKeyXpath, List<String> relayRuleKeyXpath)
        throws Exception {
        init();

        boolean isMatch = true;

        if (mediaRoomPolicyRuleElement != null) {
            List<?> ruleKeyTagFilterElementList = new ArrayList<Object>();

            Element ruleKeyTagFilterSetElement = mediaRoomPolicyRuleElement.element("RuleKeyTagFilterSet");

            if (ruleKeyTagFilterSetElement != null) {
                ruleKeyTagFilterElementList = ruleKeyTagFilterSetElement.elements("RuleKeyTagFilter");
            }           
            for (int i = 0; (ruleKeyTagFilterElementList != null) && (i < ruleKeyTagFilterElementList.size()); i++) {
                if (!isMatch) {
                    return false;
                }

                Element ruleKeyFilterElement = (Element) ruleKeyTagFilterElementList.get(i);
                int ruleCase = 0;
                try {
                	ruleCase = getCase(ruleKeyFilterElement);
                }
                catch(Exception e) {
                	System.out.println("Ignoring ruleKey=" + MRXMLUtils.elementToString(ruleKeyFilterElement));
                	e.printStackTrace();
                	continue;
                }

                switch (requestType) {
                case GET_SOURCE_REQUEST:
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, destinationList, operatorList, roomList, userRuleKeyXpath, resourceRuleKeyXpath);

                    break;

                case GET_PROFILE_REQUEST:
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, operatorList, roomList, profileRuleKeyXpath);

                    break;

                case GET_MATCHING_POLICY_REQUEST:
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, profileList, operatorList, roomList);

                    break;

                case GET_DESTINATION_REQUEST:
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, operatorList, roomList, userRuleKeyXpath, resourceRuleKeyXpath, false);

                    break;

                case GET_RELAY_REQUEST:
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, operatorList, roomList, profileRuleKeyXpath);
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, operatorList, roomList, relayRuleKeyXpath, true);

                    break;

                case GET_MEDIA_ROOM_REQUEST:
                    isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, operatorList, userRuleKeyXpath);

                    break;

                default:
                    isMatch = true;

                    break;
                }
            }
        }

       /* List<?> streamFilterNIDELList = MRXMLUtils.getListXpath(mediaRoomPolicyRuleElement, "MediaRoomStreamFilterSpec/MediaRoomStreamFilterSet/MediaStreamFilterNID");

        for (int i = 0; (streamFilterNIDELList != null) && (i < streamFilterNIDELList.size()); i++) {
            if (streamFilterNIDELList.get(i) != null) {
                Element streamFilterElement = streamFilterNIDMapping.get(((Element) streamFilterNIDELList.get(i)).getValue());
                isMatch = matchStreamFilterRuleKey(streamFilterElement, sourceList, destinationList, profileList, operatorList, roomList, requestType, userRuleKeyXpath, resourceRuleKeyXpath, profileRuleKeyXpath, relayRuleKeyXpath);

                if (!isMatch) {
                    return isMatch;
                }
            }
        }*/

        return isMatch;
    }

    public static boolean matchStreamFilterRuleKey(Element streamFilterElement, Element sourceList, Element destinationList, Element profileList, Element operatorList, Element roomList, int requestType, List<String> userRuleKeyXpath, List<String> resourceRuleKeyXpath,
        List<String> profileRuleKeyXpath, List<String> relayRuleKeyXpath) throws Exception {
    	init();
        boolean isMatch = true;

        if (streamFilterElement != null) {
            if (streamFilterElement.element("RuleKeyTagFilterSet") != null) {
                List<?> streamRuleKeyTagFilterElementList = MRXMLUtils.getListXpath(streamFilterElement, "RuleKeyTagFilterSet/RuleKeyTagFilter");

                for (int j = 0; (streamRuleKeyTagFilterElementList != null) && (j < streamRuleKeyTagFilterElementList.size()); j++) {
                    if (!isMatch) {
                        return false;
                    }

                    Element ruleKeyFilterElement = (Element) streamRuleKeyTagFilterElementList.get(j);

                    int ruleCase = 0;
                    try {
                    	ruleCase = getCase(ruleKeyFilterElement);
                    }
                    catch(Exception e) {
                    	System.out.println("Ignoring ruleKey=" + MRXMLUtils.elementToString(ruleKeyFilterElement));
                    	e.printStackTrace();
                    }

                    switch (requestType) {
                    case GET_PROFILE_REQUEST:
                        isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, operatorList, roomList, profileRuleKeyXpath);

                        break;

                    case GET_RELAY_REQUEST:
                        isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, operatorList, roomList, profileRuleKeyXpath);
                        isMatch = matchRuleKeyTagFilterCase(ruleKeyFilterElement, ruleCase, sourceList, destinationList, operatorList, roomList, relayRuleKeyXpath, true);

                        break;

                    default:
                        isMatch = true;

                        break;
                    }
                }
            }
        }

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element sourceList, Element operatorList, Element roomList, List<String> userRuleKeyXpath, List<String> resourceRuleKeyXpath, boolean isSourceFlag)
        throws Exception {
        if (sourceList == null) {
            return true;
        }

        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, sourceList, null, null, operatorList, roomList, GET_DESTINATION_REQUEST, userRuleKeyXpath, resourceRuleKeyXpath, null);

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element operatorList, List<String> roomRuleKeyXpath)
        throws Exception {
        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, null, null, null, operatorList, null, GET_MEDIA_ROOM_REQUEST, roomRuleKeyXpath, null, null);

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element destinationList, Element operatorList, Element roomList, List<String> userRuleKeyXpath, List<String> resourceRuleKeyXpath)
        throws Exception {
        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, null, destinationList, null, operatorList, roomList, GET_SOURCE_REQUEST, userRuleKeyXpath, resourceRuleKeyXpath, null);

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element sourceList, Element destinationList, Element operatorList, Element roomList, List<String> profileRuleKeyXpath, List<String> relayRuleKeyXpath)
        throws Exception {
        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, sourceList, destinationList, null, operatorList, roomList, GET_RELAY_REQUEST, null, null, profileRuleKeyXpath, relayRuleKeyXpath);

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element sourceList, Element destinationList, Element operatorList, Element roomList, List<String> profileRuleKeyXpath)
        throws Exception {
        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, sourceList, destinationList, null, operatorList, roomList, GET_PROFILE_REQUEST, null, null, profileRuleKeyXpath);

        return isMatch;
    }

    public static boolean matchRuleKeyTagFilter(Element mediaRoomPolicyRuleElement, Hashtable<String, Element> streamFilterNIDMapping, Element sourceList, Element destinationList, Element operatorList, Element roomList)
        throws Exception {
        boolean isMatch = matchRuleKeyTagFilter(mediaRoomPolicyRuleElement, streamFilterNIDMapping, sourceList, destinationList, null, operatorList, roomList, GET_MATCHING_POLICY_REQUEST, null, null, null);

        return isMatch;
    }

    /*@SuppressWarnings("unchecked")
    public static boolean matchTag(Element dstUserGroup, Element dstResourceGroup, String ruleKeyLHTagName, String ruleKeyRHTagName) {
        boolean isMatch = false;
        List<Element> tagOp1 = dstUserGroup.elements("Tag");
        List<Element> tagOp2 = dstResourceGroup.elements("Tag");
    
        for (int i = 0; i < tagOp1.size(); i++) {
            String op1Name = tagOp1.get(i).getAttributeValue("Name");
            String op1Value = tagOp1.get(i).getAttributeValue("Value");
    
            if (!op1Name.equals(ruleKeyLHTagName)) {
                continue;
            }
    
            for (int j = 0; j < tagOp2.size(); j++) {
                String op2Name = tagOp2.get(j).getAttributeValue("Name");
                String op2Value = tagOp2.get(j).getAttributeValue("Value");
    
                if (!op2Name.equals(ruleKeyRHTagName)) {
                    continue;
                }
    
                if (op1Value.equalsIgnoreCase(op2Value)) {
                    isMatch = true;
    
                    break;
                }
            }
    
            if (isMatch) {
                break;
            }
        }
    
        return isMatch;
    }*/
    public static boolean matchTag_New(Element dstUserGroup, Element dstResourceGroup, String ruleKeyLHTagName, String ruleKeyRHTagName, String operator)
        throws Exception {
        boolean isMatch = false;

        if ((dstUserGroup == null) || (dstResourceGroup == null)) {
            return false;
        }

        Document doc = MRXMLUtils.elementToDocument(dstUserGroup);
        List<String> lhAttributeValue = new ArrayList<String>();
        List<String> rhAttributeValue = new ArrayList<String>();

        for (int j = 0; j < 2; j++) {
            if (j == 1) {
                ruleKeyLHTagName = ruleKeyRHTagName;
                doc = MRXMLUtils.elementToDocument(dstResourceGroup);
            }

            if (ruleKeyLHTagName.indexOf("and") > 0) {
                String parentxpath = ruleKeyLHTagName.substring(0, ruleKeyLHTagName.indexOf("/@"));

                for (int i = 1;; i++) {
                    String parentxpathi = parentxpath + "[" + i + "]";
                    Element parentEL = MRXMLUtils.getElementXpath(doc, parentxpathi);

                    if (parentEL == null) {
                        break;
                    }

                    String attributeName = ruleKeyLHTagName.substring(ruleKeyLHTagName.indexOf("@") + 1, ruleKeyLHTagName.indexOf("="));
                    String attributeValue = MRXMLUtils.getAttributeValue(parentEL, attributeName, null);

                    String expectedAttributeValue = ruleKeyLHTagName.substring(ruleKeyLHTagName.indexOf("'") + 1, ruleKeyLHTagName.lastIndexOf("'"));

                    if ((attributeValue != null) && (expectedAttributeValue != null) && attributeValue.trim().equalsIgnoreCase(expectedAttributeValue.trim())) {
                        attributeName = ruleKeyLHTagName.substring(ruleKeyLHTagName.lastIndexOf("@") + 1);
                        attributeValue = MRXMLUtils.getAttributeValue(parentEL, attributeName, null);

                        if (attributeValue != null) {
                            if (j == 0) {
                                lhAttributeValue.add(attributeValue);
                            } else {
                                rhAttributeValue.add(attributeValue);
                            }
                        }
                    }
                }
            } else if (ruleKeyLHTagName.indexOf("@") > 0) {
                String parentxpath = ruleKeyLHTagName.substring(0, ruleKeyLHTagName.indexOf("/@"));

                for (int i = 1;; i++) {
                    String parentxpathi = parentxpath + "[" + i + "]";
                    Element parentEL = MRXMLUtils.getElementXpath(doc, parentxpathi);

                    if (parentEL == null) {
                        break;
                    }

                    String attributeName = ruleKeyLHTagName.substring(ruleKeyLHTagName.indexOf("@") + 1);
                    String attributeValue = MRXMLUtils.getAttributeValue(parentEL, attributeName, null);

                    if (attributeValue != null) {
                        if (j == 0) {
                            lhAttributeValue.add(attributeValue);
                        } else {
                            rhAttributeValue.add(attributeValue);
                        }
                    }
                }
            } else {
                Element valueElement = MRXMLUtils.getElementXpath(doc, ruleKeyLHTagName);

                if (valueElement != null) {
                    String value = MRXMLUtils.getValue(valueElement);

                    if (value != null) {
                        if (j == 0) {
                            lhAttributeValue.add(value);
                        } else {
                            rhAttributeValue.add(value);
                        }
                    }
                }
            }
        }

        for (int k = 0; k < lhAttributeValue.size(); k++) {
            int LHValue = Utils.getIntValue(lhAttributeValue.get(k).trim(), 0);

            for (int l = 0; l < rhAttributeValue.size(); l++) {
                int RHValue = Utils.getIntValue(rhAttributeValue.get(l).trim(), 0);

                if ("=".equals(operator)) {
                    if (lhAttributeValue.get(k).trim().equalsIgnoreCase(rhAttributeValue.get(l).trim())) {
                        return true;
                    }
                } else if ("!=".equals(operator)) {
                    if (!lhAttributeValue.get(k).trim().equalsIgnoreCase(rhAttributeValue.get(l).trim())) {
                        return true;
                    }
                } else if ("&lt;".equals(operator)) {
                    if (LHValue < RHValue) {
                        return true;
                    }
                } else if ("&lt;=".equals(operator)) {
                    if (LHValue <= RHValue) {
                        return true;
                    }
                } else if ("&gt;".equals(operator)) {
                    if (LHValue > RHValue) {
                        return true;
                    }
                } else if ("&gt;=".equals(operator)) {
                    if (LHValue >= RHValue) {
                        return true;
                    }
                } else {
                    return false;
                }
            }
        }

        return isMatch;
    }

    public static String getMatchOperator(String operator) throws MRException,Exception{
    	if(operator == null || operator.isEmpty()){
    		throw new MRException(PolicyManagementExceptionCodes.INVALID_OPERATOR,"Invalid Operator");
    	}
    	
        if ("&lt;".equals(operator)) {
            return "<";
        } else if ("&lt;=".equals(operator)) {
            return "<=";
        } else if ("&gt;".equals(operator)) {
            return ">";
        } else if ("&gt;=".equals(operator)) {
            return ">=";
        } else {
            return operator;
        }
    }

    public static boolean matchRuleKey(List<Element> destinationElement, List<Element> operatorElement, String operand1Group, String operand2Group, Element ruleKeyFilterElement, String rhTagName)
        throws Exception {
        boolean isMatch = false;

        for (int j = 0; (destinationElement != null) && (j < destinationElement.size()); j++) {
            Element temp = (Element) destinationElement.get(j).elements().get(1);

            if (!operand1Group.equalsIgnoreCase("User")) {
                operand1Group = temp.getName();

                if (operand1Group.equalsIgnoreCase("User")) {
                    temp = (Element) destinationElement.get(j).elements().get(0);
                    operand1Group = temp.getName();
                }
            }

            Element dstUserGroup = destinationElement.get(j).element(operand1Group);

            //System.out.println(MRXMLUtils.elementToString(destinationElement.get(j)));
            for (int k = 0; (operatorElement != null) && (k < operatorElement.size()); k++) {
                //System.out.println(MRXMLUtils.elementToString(operatorElement.get(k)));
                if (!(operand2Group.equalsIgnoreCase("User"))) {
                    temp = (Element) operatorElement.get(j).elements().get(1);
                    operand2Group = temp.getName();

                    if (operand2Group.equalsIgnoreCase("User")) {
                        temp = (Element) operatorElement.get(j).elements().get(0);
                        operand2Group = temp.getName();
                    }
                }

                Element operatorUserGroup = operatorElement.get(k).element(operand2Group);
                String ruleKeyLHTagName = ruleKeyFilterElement.element("RuleKeyLHTagName").elementText("TagName");
                String ruleKeyRHTagName = ruleKeyFilterElement.element("RuleKeyRHTagName").elementText("TagName");

                if (ruleKeyFilterElement.element("RuleKeyLHTagName").elementText("RuleKey").equalsIgnoreCase(rhTagName)) {
                    String temp1 = ruleKeyLHTagName;
                    ruleKeyLHTagName = ruleKeyRHTagName;
                    ruleKeyRHTagName = temp1;
                }

                String operator = ruleKeyFilterElement.elementText("MatchOp");

                if (matchTag_New(dstUserGroup, operatorUserGroup, ruleKeyLHTagName, ruleKeyRHTagName, operator)) {
                    return true;
                }
            }
        }

        return isMatch;
    }

    /*@SuppressWarnings("unchecked")
    public static String generateRuleKeyXpath(List<Element> destinationElement, Element ruleKeyFilterElement, String rhTagName, String groupTag) {
        StringBuffer ruleKeyXpath = null;
    
        for (int j = 0; j < destinationElement.size(); j++) {
            Element dstUserGroup = destinationElement.get(j).element(groupTag);
            List<Element> tagElement = dstUserGroup.elements("Tag");
            String ruleKeyLHTagName = ruleKeyFilterElement.element("RuleKeyLHTagName").elementText("TagName");
            String ruleKeyRHTagName = ruleKeyFilterElement.element("RuleKeyRHTagName").elementText("TagName");
            String matchOp = ruleKeyFilterElement.elementText("MatchOp");
    
            if (ruleKeyFilterElement.element("RuleKeyLHTagName").elementText("RuleKey").equals(rhTagName)) {
                String temp = ruleKeyLHTagName;
                ruleKeyLHTagName = ruleKeyRHTagName;
                ruleKeyRHTagName = temp;
            }
    
            for (int k = 0; k < tagElement.size(); k++) {
                if (ruleKeyLHTagName.equals(tagElement.get(k).getAttributeValue("Name"))) {
                    if (ruleKeyXpath == null) {
                        ruleKeyXpath = new StringBuffer("(");
                    } else {
                        ruleKeyXpath.append(" or ");
                    }
    
                    ruleKeyXpath.append("( " + ruleKeyRHTagName + matchOp +"'" +tagElement.get(k).getAttributeValue("Value") + "'" + ")");
    
                    if (matchOp.equalsIgnoreCase("EqualTo")) {
                        //ruleKeyXpath.append("(Groups/Tag/@Name = '" + ruleKeyRHTagName + "' and  " + "Groups/Tag/@Value = '" +
    
                        //tagElement.get(k).getAttributeValue("Value") + "')");
                        ruleKeyXpath.append("( " + ruleKeyRHTagName + "='" + tagElement.get(k).getAttributeValue("Value") + "'" + ")");
                    } else {
                        ruleKeyXpath.append("( " + ruleKeyRHTagName + "!='" + tagElement.get(k).getAttributeValue("Value") + "'" + ")");
                    }
                }
            }
        }
    
        if (ruleKeyXpath != null) {
            ruleKeyXpath.append(")");
    
            return ruleKeyXpath.toString();
        } else {
            return null;
        }
    }*/
    public static String generateRuleKeyXpath_New(List<Element> destinationElement, Element ruleKeyFilterElement, String rhTagName, String groupTag)
        throws Exception {
        String ruleKeyLHTagName = ruleKeyFilterElement.element("RuleKeyLHTagName").elementText("TagName");
        String ruleKeyRHTagName = ruleKeyFilterElement.element("RuleKeyRHTagName").elementText("TagName");
        String matchOp = ruleKeyFilterElement.elementText("MatchOp");
        StringBuffer ruleKeyXpath = null;

        if (ruleKeyFilterElement.element("RuleKeyLHTagName").elementText("RuleKey").equals(rhTagName)) {
            String temp = ruleKeyLHTagName;
            ruleKeyLHTagName = ruleKeyRHTagName;
            ruleKeyRHTagName = temp;
        }

        for (int j = 0; j < destinationElement.size(); j++) {
            if (!groupTag.equalsIgnoreCase("User")) {
                Element temp = (Element) destinationElement.get(j).elements().get(1);
                groupTag = temp.getName();

                if (groupTag.equalsIgnoreCase("User")) {
                    temp = (Element) destinationElement.get(j).elements().get(0);
                    groupTag = MRXMLUtils.getValue(temp.getDocument().getRootElement());
                }
            }

            Document destEl = MRXMLUtils.elementToDocument(destinationElement.get(j).element(groupTag));

            if (ruleKeyLHTagName.indexOf("and") > 0) {
                String parentxpath = ruleKeyLHTagName.substring(0, ruleKeyLHTagName.indexOf("/@"));

                for (int i = 1;; i++) {
                    String parentxpathi = parentxpath + "[" + i + "]";

                    Element parentEL = MRXMLUtils.getElementXpath(destEl, parentxpathi);

                    if (parentEL == null) {
                        break;
                    }

                    String attributeName = ruleKeyLHTagName.substring(ruleKeyLHTagName.indexOf("@") + 1, ruleKeyLHTagName.indexOf("="));
                    String attributeValue = MRXMLUtils.getAttributeValue(parentEL, attributeName, null);
                    String expectedAttributeValue = ruleKeyLHTagName.substring(ruleKeyLHTagName.indexOf("'") + 1, ruleKeyLHTagName.lastIndexOf("'"));

                    if ((attributeValue != null) && (expectedAttributeValue != null) && attributeValue.trim().equalsIgnoreCase(expectedAttributeValue.trim())) {
                        attributeName = ruleKeyLHTagName.substring(ruleKeyLHTagName.lastIndexOf("@") + 1);
                        attributeValue = MRXMLUtils.getAttributeValue(parentEL, attributeName, null);

                        if (ruleKeyXpath == null) {
                            ruleKeyXpath = new StringBuffer("(");
                        } else {
                            ruleKeyXpath.append(" or ");
                        }
                        
                        ruleKeyXpath.append("( " + ruleKeyRHTagName + getMatchOperator(matchOp) + "'" + attributeValue + "'" + ")");

                        /* if (matchOp.equalsIgnoreCase("EqualTo")) {
                             ruleKeyXpath.append("( " + ruleKeyRHTagName + "='" + attributeValue + "'" + ")");
                         } else {
                             ruleKeyXpath.append("( " + ruleKeyRHTagName + "!='" + attributeValue + "'" + ")");
                         }*/
                    }
                }
            } else if (ruleKeyLHTagName.indexOf("@") > 0) {
                String parentxpath = ruleKeyLHTagName.substring(0, ruleKeyLHTagName.indexOf("/@"));

                for (int i = 1;; i++) {
                    String parentxpathi = parentxpath + "[" + i + "]";
                    Element parentEL = MRXMLUtils.getElementXpath(destEl, parentxpathi);

                    if (parentEL == null) {
                        break;
                    }

                    String attributeName = ruleKeyLHTagName.substring(ruleKeyLHTagName.indexOf("@") + 1);
                    String attributeValue = MRXMLUtils.getAttributeValue(parentEL, attributeName, null);

                    if (ruleKeyXpath == null) {
                        ruleKeyXpath = new StringBuffer("(");
                    } else {
                        ruleKeyXpath.append(" or ");
                    }

                    ruleKeyXpath.append("( " + ruleKeyRHTagName +  getMatchOperator(matchOp) + "'" + attributeValue + "'" + ")");

                    /*if (matchOp.equalsIgnoreCase("EqualTo")) {
                        ruleKeyXpath.append("( " + ruleKeyRHTagName + "='" + attributeValue + "'" + ")");
                    } else {
                        ruleKeyXpath.append("( " + ruleKeyRHTagName + "!='" + attributeValue + "'" + ")");
                    }*/
                }
            } else {
                Element valueElement = MRXMLUtils.getElementXpath(destEl, ruleKeyLHTagName);

                if (valueElement != null) {
                    String value = MRXMLUtils.getValue(valueElement);

                    if (ruleKeyXpath == null) {
                        ruleKeyXpath = new StringBuffer("(");
                    } else {
                        ruleKeyXpath.append(" or ");
                    }

                    ruleKeyXpath.append("( " + ruleKeyRHTagName +  getMatchOperator(matchOp) + "'" + value + "'" + ")");

                    /* if (matchOp.equalsIgnoreCase("EqualTo")) {
                         ruleKeyXpath.append("( " + ruleKeyRHTagName + "='" + value + "'" + ")");
                     } else {
                         ruleKeyXpath.append("( " + ruleKeyRHTagName + "!='" + value + "'" + ")");
                     }*/
                }
            }
        }

        if (ruleKeyXpath != null) {
            ruleKeyXpath.append(")");

            return ruleKeyXpath.toString();
        } else {
            return null;
        }
    }

    /* */
    /**************************************************/
    /*
    public static Element findMatchingRoomGroupFilter(Element roomGroup, String serviceName, MRRequest request, MRDB schema, MRLog log)
    throws Exception {
    String[] whereXPath = null;
    MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
    String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
    whereXPath = getXPathforTagFilter(roomGroup);
    
    Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
    
    if ((whereXPath != null) && (whereXPath.length != 0) && (whereXPath[0] != null)) {
    Element whereAssistXML = MRXMLUtils.newElement("WhereAssistXML");
    responseEL = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL, null, 1, whereAssistXML, whereXPath, request, schema, log);
    
    //MRXMLDBUtils.findChildElements(parentNID, responseEL , null, 1, whereAssistXML, whereXPath, request, schema, log);
    //System.out.println(MRXMLUtils.elementToString(responseEL) );
    }
    
    return responseEL;
    }*/
    public static List<String> generateXPathforTagFilter(List<Element> filterList) throws MRException,Exception{
        return generateXPathforTagFilter(filterList, true);
    }

    public static List<String> generateXPathforTagFilter(List<Element> filterList, boolean allCheck) throws MRException,Exception {
        if (filterList == null) {
            return null;
        }

        List<String> filterXPathList = new ArrayList<String>();

        for (int i = 0; i < filterList.size(); i++) {
            Element filter = filterList.get(i);
            String filterXPath = null;

            try {
                //System.out.println(MRXMLUtils.elementToString(filter));
            } catch (Exception e) {
                // TODO Auto-generated catch block
                e.printStackTrace();
            }

            if (filter.elementText("Name").trim().equals("_ALL_")) {
                if (!allCheck) {
                    continue;
                }

                filterXPath = null;
            } else {
                filterXPath = generateFilterXpath(filterList.get(i));
            }

            filterXPathList.add(filterXPath);
        }

        if (filterXPathList.size() == 0) {
            return null;
        } else {
            return filterXPathList;
        }
    }

    /*@SuppressWarnings("unchecked")
    public static String[] getXPathforTagFilter(Element roomGroup, boolean allCheck) {
        if (roomGroup == null) {
            return null;
        }
    
        List<Element> tagList = roomGroup.elements("Tag");
        String name = null;
        String value = null;
    
        if (tagList == null) {
            return null;
        }
    
        StringBuffer xpath = new StringBuffer();
    
        for (int i = 0; i < tagList.size(); i++) {
            if (i != 0) {
                xpath.append(" or ");
            }
    
            name = tagList.get(i).getAttributeValue("Name");
            value = tagList.get(i).getAttributeValue("Value");
    
            if ((name == null) || "".equals(name.trim()) || (value == null) || "".equals(value.trim())) {
                continue;
            }
    
            xpath.append("(");
            xpath.append(buildCondition("TagFilterSet/TagFilter/LHTagName", name, true));
            xpath.append(" and ");
            xpath.append(buildCondition("TagFilterSet/TagFilter/MatchOp", "=", true));
            xpath.append(" and ");
            xpath.append(buildCondition("TagFilterSet/TagFilter/RHRegexp", value, true));
            xpath.append(")");
            xpath.append(" or ");
            xpath.append("(");
            xpath.append(buildCondition("TagFilterSet/TagFilter/LHTagName", name, true));
            xpath.append(" and ");
            xpath.append(buildCondition("TagFilterSet/TagFilter/MatchOp", "!=", true));
            xpath.append(" and ");
            xpath.append(buildCondition("TagFilterSet/TagFilter/RHRegexp", value, false));
            xpath.append(")");
        }
    
        if (allCheck) {
            xpath.append(" or ");
            xpath.append("(");
            xpath.append(buildCondition("Name", PolicyAdminConstants.ALL_FILTER, true));
            xpath.append(")");
        }
    
        if (xpath != null) {
            String xPathString = buildXPath("TagObjectFilter", xpath.toString());
    
            //System.out.println(xPathString);
            String[] xpathArray = new String[1];
            xpathArray[0] = xPathString;
    
            return xpathArray;
        }
    
        return null;
    }*/

    /*public static String[] getXPathforTagFilter(Element roomGroup) {
        return getXPathforTagFilter(roomGroup, true);
    }
    */
    public static String getXPathforSrcDstFilter(List<String> userNIDList, List<String> resourceNIDList) {
        StringBuffer userXpath = null;
        StringBuffer resourceXpath = null;

        //xpath = new StringBuffer("(");

        //xpath.append(buildCondition("UserFilterNID", "", true));
        for (int i = 0; (userNIDList != null) && (i < userNIDList.size()); i++) {
            if ((userNIDList.get(i) != null) || "".equals(userNIDList.get(i).trim())) {
                if (userXpath == null) {
                    userXpath = new StringBuffer("(");
                } else {
                    userXpath.append(" or ");
                }

                userXpath.append(buildCondition("UserFilterNID", userNIDList.get(i), true));
            }
        }

        if (userXpath != null) {
            userXpath.append(")");
        }

        //xpath.append(buildCondition("MediaStreamResourceFilterNID", "", true));
        for (int i = 0; (resourceNIDList != null) && (i < resourceNIDList.size()); i++) {
            if ((resourceNIDList.get(i) != null) || "".equals(resourceNIDList.get(i).trim())) {
                if (resourceXpath == null) {
                    resourceXpath = new StringBuffer("(");
                } else {
                    resourceXpath.append(" or ");
                }

                resourceXpath.append(buildCondition("MediaStreamResourceFilterNID", resourceNIDList.get(i), true));
            }
        }

        if (resourceXpath != null) {
            resourceXpath.append(")");
        }

        //System.out.println(userXpath.toString());
        if ((userXpath != null) && (resourceXpath != null)) {
            return (userXpath + " and " + resourceXpath);
        } else if (userXpath != null) {
            return userXpath.toString();
        } else if (resourceXpath != null) {
            return resourceXpath.toString();
        } else {
            return null;
        }
    }

    public static String getXPathforObjectFilter(List<String> profileNIDList, String parentTag) {
        StringBuffer xpath = null;

        //xpath = new StringBuffer("(");
        //xpath.append(buildCondition(parentTag, "", true));
        for (int i = 0; (profileNIDList != null) && (i < profileNIDList.size()); i++) {
            if ((profileNIDList.get(i) != null) || "".equals(profileNIDList.get(i).trim())) {
                if (xpath == null) {
                    xpath = new StringBuffer("(");
                } else {
                    xpath.append(" or ");
                }

                xpath.append(buildCondition(parentTag, profileNIDList.get(i), true));
            }
        }

        if (xpath != null) {
            xpath.append(")");
        }

        //System.out.println(xpath.toString());
        if (xpath != null) {
            return xpath.toString();
        } else {
            return null;
        }
    }

    public static String getXPathforStreamFilter(List<String> xpath, String parentTag, boolean isSource) {
        return getXPathforStreamFilter(xpath, parentTag, false, isSource);
    }

    /* public static String getXPathforStreamFilter(List<String> xpath, String parentTag) {
         return getXPathforStreamFilter(xpath, parentTag, false, false);
     }*/
    public static String getXPathforStreamFilter(List<String> xpath, String parentTag, boolean isRelay, boolean isSource) {
        StringBuffer streamFilterXPathValue = null;

        if (xpath != null) {
            for (int i = 0; (xpath != null) && (i < xpath.size()); i++) {
                if ((xpath.get(i) != null) || "".equals(xpath.get(i))) {
                    if (streamFilterXPathValue == null) {
                        streamFilterXPathValue = new StringBuffer();
                    } else {
                        streamFilterXPathValue.append(" or ");
                    }

                    streamFilterXPathValue.append("(");
                    streamFilterXPathValue.append(xpath.get(i));
                    streamFilterXPathValue.append(")");
                }
            }
        }

        if (streamFilterXPathValue != null) {
            StringBuffer newXPath = streamFilterXPathValue;

            // If it is relay check then check relay filter is empty or not 
            if (isRelay) {
                newXPath = new StringBuffer();
                newXPath.append("( " + streamFilterXPathValue + ")");
                newXPath.append(" and ");
                newXPath.append("  ( //" + PolicyAdminConstants.RELAY_FILTER_SET + "/" + PolicyAdminConstants.RELAY_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID + " != '' )");
            } else if (!isSource) {
                newXPath = new StringBuffer();
                newXPath.append("( " + streamFilterXPathValue + ")");
                newXPath.append(" and ");
                newXPath.append("  ( //" + PolicyAdminConstants.RELAY_FILTER_SET + " = '' )");
            }

            return buildXPath(parentTag, newXPath.toString());
        } else {
            return null;
        }
    }

    /*public static Element getMatchingStreamPolicy(List<String> destinationXpath, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        Element responseEl = MediaRoomPolicyHelper.getMatchingStreamPolicy(null, destinationXpath, null, serviceName, request, schema, log);
    
        return responseEl;
    }*/
    public static Element getMatchingStreamPolicy(List<String> destinationXpath, boolean isRelay, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        Element responseEl = MediaRoomPolicyHelper.getMatchingStreamPolicy(null, destinationXpath, null, isRelay, false, serviceName, request, schema, log);

        return responseEl;
    }

    public static Element getMatchingStreamPolicy(List<String> sourceXpath, List<String> destinationXpath, boolean isRelay, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        Element responseEl = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, null, isRelay, false, serviceName, request, schema, log);

        return responseEl;
    }

    public static Element getMatchingStreamPolicy(List<String> sourceXpath, List<String> destinationXpath, boolean isRelay, boolean isSource, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        Element responseEl = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, null, isRelay, isSource, serviceName, request, schema, log);

        return responseEl;
    }

    /* public static Element getMatchingStreamPolicy(List<String> sourceXpath, List<String> destinationXpath, String serviceName, MRRequest request, MRDB schema, MRLog log)
         throws Exception {
         Element responseEl = MediaRoomPolicyHelper.getMatchingStreamPolicy(sourceXpath, destinationXpath, null, serviceName, request, schema, log);
    
         return responseEl;
     }*/

    /* public static Element getMatchingStreamPolicy(List<String> sourceXpath, List<String> destinationXpath, List<String> profileXpath, String serviceName, MRRequest request, MRDB schema, MRLog log)
         throws Exception {
         return getMatchingStreamPolicy(sourceXpath, destinationXpath, profileXpath, false, false, serviceName, request, schema, log);
     }*/
    public static Element getMatchingStreamPolicy(List<String> sourceXpath, List<String> destinationXpath, List<String> profileXpath, boolean isSource, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        return getMatchingStreamPolicy(sourceXpath, destinationXpath, profileXpath, false, isSource, serviceName, request, schema, log);
    }

    public static Element getMatchingStreamPolicy(List<String> sourceXpath, List<String> destinationXpath, List<String> profileXpath, boolean isRelay, boolean isSource, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        String[] whereXPath = null;
        String tempSourceXPath = null;
        String tempDestinationXPath = null;
        String tempProfileXPath = null;

        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element whereAssistXML = MRXMLUtils.newElement("WhereAssistXML");
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE + "List");
        tempSourceXPath = getXPathforStreamFilter(sourceXpath, "SrcFilter", isRelay, isSource);
        //System.out.println(tempSourceXPath);
        tempDestinationXPath = getXPathforStreamFilter(destinationXpath, "DstFilter", isRelay, isSource);
        //System.out.println(tempDestinationXPath);
        tempProfileXPath = getXPathforStreamFilter(profileXpath, "ProfileFilter", isRelay, isSource);

        //System.out.println(tempProfileXPath);
        if ((tempSourceXPath != null) && (tempDestinationXPath != null) && (tempProfileXPath != null)) {
            whereXPath = new String[3];
            whereXPath[0] = tempSourceXPath;
            whereXPath[1] = tempDestinationXPath;
            whereXPath[2] = tempProfileXPath;
        } else if ((tempSourceXPath != null) && (tempDestinationXPath != null)) {
            whereXPath = new String[2];
            whereXPath[0] = tempSourceXPath;
            whereXPath[1] = tempDestinationXPath;
        } else if ((tempSourceXPath != null) && (tempProfileXPath != null)) {
            whereXPath = new String[2];
            whereXPath[0] = tempSourceXPath;
            whereXPath[1] = tempProfileXPath;
        } else if ((tempDestinationXPath != null) && (tempProfileXPath != null)) {
            whereXPath = new String[2];
            whereXPath[0] = tempDestinationXPath;
            whereXPath[1] = tempProfileXPath;
        } else if (tempDestinationXPath != null) {
            whereXPath = new String[1];
            whereXPath[0] = tempDestinationXPath;
        } else if (tempSourceXPath != null) {
            whereXPath = new String[1];
            whereXPath[0] = tempSourceXPath;
        } else if (tempProfileXPath != null) {
            whereXPath = new String[1];
            whereXPath[0] = tempProfileXPath;
        }

        if ((whereXPath != null) && !"".equals(whereXPath[0].trim())) {
            responseEL = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, responseEL, null, 1, whereAssistXML, whereXPath, request, schema, log);
        }

        return responseEL;
    }

    public static String buildCondition(String tagName, String value, boolean isEqualToCondition) {
        if (isEqualToCondition) {
            return tagName + "= '" + value + "'";
        } else {
            return tagName + "!= '" + value + "'";
        }
    }

    public static String buildXPath(String parentTag, String xpathCondition) {
        if ((parentTag == null) || (xpathCondition == null) || "".equals(xpathCondition)) {
            return null;
        }

        return "//" + parentTag + "[" + xpathCondition + "]";
    }

    /************/
    @SuppressWarnings("unchecked")
    public static List<Element> findMatchingRoomGroupFilter_New(Element destination, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));

        Element filterListElement = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");

        if (destination == null) {
            return null;
        }

        filterListElement = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, filterListElement, null, 1, null, null, request, schema, log);

        if (filterListElement == null) {
            return null;
        }

        final List<Element> filterList = filterListElement.elements("TagObjectFilter");
        List<String> filterXpathList = generateXPathforTagFilter(filterList);

        if (filterXpathList == null) { //No filter found in database 

            return null;
        }

        List<Element> roomfilterList = null;

        String resourceType = null;

        List<Element> roomList = destination.elements();

        if ((roomList != null) && (roomList.size() > 0)) {
            resourceType = roomList.get(0).getName();
        } else {
            return null;
        }

        for (int j = 0; j < filterXpathList.size(); j++) {
            //System.out.println(filterXpathList.get(j));
            List<String> xpath = new ArrayList<String>();

            if ((filterXpathList.get(j) != null) && (filterXpathList.get(j).trim().length() != 0)) {
                xpath.add("//" + resourceType + "[" + filterXpathList.get(j) + "]");
            } else {
                xpath.add(filterXpathList.get(j));
            }

            List<Element> tempList = matchXPath(roomList, xpath, null);

            if ((tempList != null) && (tempList.size() > 0)) {
                if (roomfilterList == null) {
                    roomfilterList = new ArrayList<Element>();
                }

                //if(checkFilterType(filterList.get(j), "User")) {
                roomfilterList.add((Element) filterList.get(j).clone());

                //}
            }
        }

        return roomfilterList;
    }

    private static List<Element> getfiletListWithOutAll(List<Element> filterList) {
        List<Element> filterListWithOutAll = new ArrayList<Element>();

        for (int i = 0; i < filterList.size(); i++) {
            Element filter = filterList.get(i);

            if (!filter.elementText("Name").trim().equals("_ALL_")) {
                filterListWithOutAll.add(filter);
            }
        }

        return filterListWithOutAll;
    }

    @SuppressWarnings("unchecked")
    public static Element[][] findMatchingDestinationGroupFilter_New(List<Element> destinationList, String serviceName, boolean allCheck, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));

        Element filterListElement = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element[][] responseEL = new Element[30][30];

        if (destinationList == null) {
            return null;
        }

        filterListElement = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, filterListElement, null, 1, null, null, request, schema, log);

        if (filterListElement == null) {
            return null;
        }

        final List<Element> filterList = filterListElement.elements("TagObjectFilter");
        List<Element> filterwithOutAllcheck = null;

        if (!allCheck) {
            filterwithOutAllcheck = getfiletListWithOutAll(filterList);
        }

        List<String> resourceXpathList = generateXPathforTagFilter(filterList, allCheck);
        List<String> userXpathList = generateXPathforTagFilter(filterList);

        if ((resourceXpathList == null) || (userXpathList == null)) { //No filter found in database 

            return null;
        }

        for (int i = 0; i < destinationList.size(); i++) {
            String resourceType = null;
            Element destination = destinationList.get(i);
            List<Element> userList = destination.elements("User");

            //List<Element> resourceList = destination.elements("AssetResource");
            List<Element> resourceList = destination.elements("AssetResource");
            resourceType = getResourceType(resourceList, destination);
            resourceList = destination.elements(resourceType);

            List<Element> userFilterList = new ArrayList<Element>();
            List<Element> resourceFilterList = new ArrayList<Element>();

            //List<String> xpath = new ArrayList<String>();
            List<String> xpath = null;

            for (int j = 0; j < userXpathList.size(); j++) {
                List<Element> tempUserList = null;
                xpath = new ArrayList<String>();

                if ((userXpathList.get(j) != null) && (userXpathList.get(j).trim().length() != 0)) {
                    xpath.add("//" + "User" + "[" + userXpathList.get(j) + "]");
                } else {
                    xpath.add(userXpathList.get(j));
                }

                tempUserList = matchXPath(userList, xpath, null);

                if ((tempUserList != null) && (tempUserList.size() > 0)) {
                    //if(checkFilterType(filterList.get(j), "User")) {
                    userFilterList.add((Element) filterList.get(j).clone());

                    //}
                }
            }

            for (int j = 0; j < resourceXpathList.size(); j++) {
                //System.out.println(filterXpathList.get(j));
                List<Element> tempResourceList = null;

                xpath = new ArrayList<String>();

                if ((resourceXpathList.get(j) != null) && (resourceXpathList.get(j).trim().length() != 0)) {
                    xpath.add("//" + resourceType + "[" + resourceXpathList.get(j) + "]");
                } else {
                    xpath.add(resourceXpathList.get(j));
                }

                tempResourceList = matchXPath(resourceList, xpath, null);

                if ((tempResourceList != null) && (tempResourceList.size() > 0)) {
                    if (!allCheck) {
                        resourceFilterList.add((Element) filterwithOutAllcheck.get(j).clone());
                    } else {
                        //if(checkFilterType(filterList.get(j), "AssetResource") || checkFilterType(filterList.get(j), "MediaStreamPortResource")) {
                        resourceFilterList.add((Element) filterList.get(j).clone());
                    }

                    //}
                }
            }

            if ((userFilterList != null) && (userFilterList.size() > 0) && (resourceFilterList != null) && (resourceFilterList.size() > 0)) {
                Element userFilter = generateDocumentFromList(userFilterList);
                Element resourceFilter = generateDocumentFromList(resourceFilterList);
                responseEL[i][0] = userFilter;
                responseEL[i][1] = resourceFilter;
            }
        }

        return responseEL;
    }

    @SuppressWarnings("unchecked")
    private static String getResourceType(List<Element> resourceList, Element destination)
        throws Exception {
        String resourceType = "AssetResource";

        if ((resourceList == null) || (resourceList.size() == 0)) {
            resourceList = destination.elements("MediaStreamRelayPortResource");
            resourceType = "MediaStreamRelayPortResource";

            if ((resourceList == null) || (resourceList.size() == 0)) {
                resourceList = destination.elements("MediaStreamDstPortResource");
                resourceType = "MediaStreamDstPortResource";
            }

            if ((resourceList == null) || (resourceList.size() == 0)) {
                resourceList = destination.elements("MediaStreamSrcPortResource");
                resourceType = "MediaStreamSrcPortResource";
            }

            // Added support for IO Ports
            if ((resourceList == null) || (resourceList.size() == 0)) {
                resourceList = destination.elements("MediaStreamIOPortResource");
                resourceType = "MediaStreamIOPortResource";
            }
            //NOTE: any resource type check add before this statement  
            if ((resourceList == null) || (resourceList.size() == 0)) {
                //resourceList = destination.elements("MediaStorePortResource");
                resourceType = "MediaStorePortResource";
            }
        }

        return resourceType;
    }

    /***********/
    //Newly added method 
    //TBD All check 
    @SuppressWarnings("unchecked")
    public static Element[][] findMatchingDestinationGroupFilter_New(List<Element> destinationList, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));

        Element filterListElement = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element[][] responseEL = new Element[30][30];

        if (destinationList == null) {
            return null;
        }

        filterListElement = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, filterListElement, null, 1, null, null, request, schema, log);

        if (filterListElement == null) {
            return null;
        }

        final List<Element> filterList = filterListElement.elements("TagObjectFilter");
        List<String> filterXpathList = generateXPathforTagFilter(filterList);

        if (filterXpathList == null) { //No filter found in database 

            return null;
        }

        for (int i = 0; i < destinationList.size(); i++) {
            String resourceType = null;
            Element destination = destinationList.get(i);
            List<Element> userList = destination.elements("User");

            //List<Element> resourceList = destination.elements("AssetResource");
            List<Element> resourceList = destination.elements("AssetResource");
            resourceType = "AssetResource";

            if ((resourceList == null) || (resourceList.size() == 0)) {
                resourceList = destination.elements("MediaStreamRelayPortResource");
                resourceType = "MediaStreamRelayPortResource";

                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStreamDstPortResource");
                    resourceType = "MediaStreamDstPortResource";
                }

                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStreamSrcPortResource");
                    resourceType = "MediaStreamSrcPortResource";
                }

                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStorePortResource");
                    resourceType = "MediaStorePortResource";
                }

                // Added support for IO Port Resource
                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStreamIOPortResource");
                    resourceType = "MediaStreamIOPortResource";
                }
            }

            List<Element> userFilterList = new ArrayList<Element>();
            List<Element> resourceFilterList = new ArrayList<Element>();

            for (int j = 0; j < filterXpathList.size(); j++) {
                //System.out.println(filterXpathList.get(j));
                List<Element> tempUserList = null;
                List<Element> tempResourceList = null;

                List<String> xpath = new ArrayList<String>();

                if ((filterXpathList.get(j) != null) && (filterXpathList.get(j).trim().length() != 0)) {
                    xpath.add("//" + resourceType + "[" + filterXpathList.get(j) + "]");
                } else {
                    xpath.add(filterXpathList.get(j));
                }

                tempResourceList = matchXPath(resourceList, xpath, null);
                xpath = new ArrayList<String>();

                if ((filterXpathList.get(j) != null) && (filterXpathList.get(j).trim().length() != 0)) {
                    xpath.add("//" + "User" + "[" + filterXpathList.get(j) + "]");
                } else {
                    xpath.add(filterXpathList.get(j));
                }

                tempUserList = matchXPath(userList, xpath, null);

                if ((tempUserList != null) && (tempUserList.size() > 0)) {
                    //if(checkFilterType(filterList.get(j), "User")) {
                    userFilterList.add((Element) filterList.get(j).clone());

                    //}
                }

                if ((tempResourceList != null) && (tempResourceList.size() > 0)) {
                    //if(checkFilterType(filterList.get(j), "AssetResource") || checkFilterType(filterList.get(j), "MediaStreamPortResource")) {
                    resourceFilterList.add((Element) filterList.get(j).clone());

                    //}
                }
            }

            if ((userFilterList != null) && (userFilterList.size() > 0) && (resourceFilterList != null) && (resourceFilterList.size() > 0)) {
                Element userFilter = generateDocumentFromList(userFilterList);
                Element resourceFilter = generateDocumentFromList(resourceFilterList);
                responseEL[i][0] = userFilter;
                responseEL[i][1] = resourceFilter;
            }
        }

        return responseEL;
    }

    /*@SuppressWarnings("unchecked")
    public static Element[][] findMatchingDestinationGroupFilter_New(List<Element> destinationList, String serviceName, boolean isAll, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
    
        Element filterListElement = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element[][] responseEL = new Element[30][30];
    
        if (destinationList == null) {
            return null;
        }
    
        filterListElement = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, filterListElement, null, 1, null, null, request, schema, log);
    
        if (filterListElement == null) {
            return null;
        }
    
        final List<Element> filterList = filterListElement.elements("TagObjectFilter");
        List<String> filterXpathList = generateXPathforTagFilter(filterList);
         if (filterXpathList == null) { //No filter found in database
    
            return null;
        }
    
        for (int i = 0; i < destinationList.size(); i++) {
            Element destination = (Element) destinationList.get(i);
            List<Element> userList = destination.elements("User");
            List<Element> resourceList = destination.elements("AssetResource");
            String resourceType = "AssetResource";
    
            if ((resourceList == null) || (resourceList.size() == 0)) {
                resourceList = destination.elements("MediaStreamRelayPortResource");
                resourceType = "MediaStreamRelayPortResource";
    
                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStreamDstPortResource");
                    resourceType = "MediaStreamDstPortResource";
                }
    
                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStreamSrcPortResource");
                    resourceType = "MediaStreamSrcPortResource";
                }
    
                if ((resourceList == null) || (resourceList.size() == 0)) {
                    resourceList = destination.elements("MediaStorePortResource");
                    resourceType = "MediaStorePortResource";
                }
            }
    
            List<Element> userFilterList = new ArrayList();
            List<Element> resourceFilterList = new ArrayList();
    
            for (int j = 0; j < filterXpathList.size(); j++) {
                List<Element> tempUserList = null;
                List<Element> tempResourceList = null;
    
                List<String> xpath = new ArrayList();
    
                if (filterXpathList.get(j) != null) {
                    xpath.add("//" + resourceType + "[" + filterXpathList.get(j) + "]");
                    tempResourceList = matchXPath(resourceList, xpath, null);
                    xpath = new ArrayList();
                    xpath.add("//" + "User" + "[" + filterXpathList.get(j) + "]");
                    tempUserList = matchXPath(userList, xpath, null);
    
                    if ((tempUserList != null) && (tempUserList.size() > 0)) {
                        //if(checkFilterType(filterList.get(j), "User")) {
                        userFilterList.add((Element) filterList.get(j).clone());
    
                        //}
                    }
    
                    if ((tempResourceList != null) && (tempResourceList.size() > 0)) {
                        //if(checkFilterType(filterList.get(j), "AssetResource") || checkFilterType(filterList.get(j), "MediaStreamPortResource")) {
                        resourceFilterList.add((Element) filterList.get(j).clone());
    
                        //}
                    }
                } else {
                    if (checkFilterType(filterList.get(j), "User")) {
                        xpath.add(filterXpathList.get(j));
                        userFilterList.add((Element) filterList.get(j).clone());
                    }
                }
            }
    
            if ((userFilterList != null) && (userFilterList.size() > 0) && (resourceFilterList != null) && (resourceFilterList.size() > 0)) {
                Element userFilter = generateDocumentFromList(userFilterList);
                Element resourceFilter = generateDocumentFromList(resourceFilterList);
                responseEL[i][0] = userFilter;
                responseEL[i][1] = resourceFilter;
            }
        }
    
        return responseEL;
    }*/
    public static Element generateDocumentFromList(List<Element> tempFilterList) {
        if (tempFilterList == null) {
            return null;
        }

        Element objEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");

        for (int i = 0; i < tempFilterList.size(); i++) {
            objEL.add(tempFilterList.get(i).detach());
        }

        return objEL;
    }

    /*public static Element[][] findMatchingDestinationGroupFilter(List<Element> destinationList, boolean allCheck, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws Exception {
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element[][] responseEL = new Element[30][30];
    
        if (destinationList == null) {
            return null;
        }
    
        for (int i = 0; i < destinationList.size(); i++) {
            String[] whereXPathForUserGroup = null;
            String[] whereXPathForResourceGroup = null;
            @SuppressWarnings("unused")
            String whereXPathUserGroup = null;
            @SuppressWarnings("unused")
            String whereXPathResourceGroup = null;
            Element destination = (Element) destinationList.get(i);
    
            Element userGroup = destination.element("UserGroup");
            Element resourceGroup = destination.element("ResourceGroup");
            whereXPathForUserGroup = getXPathforTagFilter(userGroup);
    
            if (allCheck) {
                //System.out.println(whereXPathForUserGroup[0]);
                whereXPathForResourceGroup = getXPathforTagFilter(resourceGroup);
            } else {
                whereXPathForResourceGroup = getXPathforTagFilter(resourceGroup, false);
            }
    
            //System.out.println(whereXPathForResourceGroup[0]);
            Element whereAssistXML = MRXMLUtils.newElement("WhereAssistXML");
            responseEL[i][0] = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
            responseEL[i][1] = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
    
            if (whereXPathForUserGroup != null) {
                responseEL[i][0] = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL[i][0], null, 1, whereAssistXML,
                        whereXPathForUserGroup, request, schema, log);
    
                //MRXMLDBUtils.findChildElements(parentNID, responseEL[i][0] , null, 1, whereAssistXML, whereXPathForUserGroup, request, schema, log);
                //System.out.println(MRXMLUtils.elementToString(responseEL[i][0]));
            }
    
            if (whereXPathForResourceGroup != null) {
                responseEL[i][1] = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, responseEL[i][1], null, 1, whereAssistXML,
                        whereXPathForResourceGroup, request, schema, log);
    
                //MRXMLDBUtils.findChildElements(parentNID, responseEL[i][1] , null, 1, whereAssistXML, whereXPathForResourceGroup, request, schema, log);
                //System.out.println(MRXMLUtils.elementToString(responseEL[i][1]));
            }
        }
    
        return responseEL;
    }*/

    /* public static Element[][] findMatchingDestinationGroupFilter(List<Element> destinationList, String serviceName, MRRequest request, MRDB schema, MRLog log)
         throws Exception {
         return findMatchingDestinationGroupFilter(destinationList, true, serviceName, request, schema, log);
     }*/
    public static List<Element> matchXPath(List<Element> policyObjectList, List<String> xpathList, String[] xpathArray)
        throws Exception {
        List<Element> matchingPolicyObjectList = new ArrayList<Element>();
        boolean isMatch = true;

        if ((xpathList == null) && (xpathArray.length == 0)) {
            return policyObjectList;
        }

        for (int i = 0; (policyObjectList != null) && (i < policyObjectList.size()); i++) {
            isMatch = true;

            for (int j = 0; (xpathList != null) && (j < xpathList.size()); j++) {
                if ((xpathList.get(j) != null) && !("").equals(xpathList.get(j).trim())) {
                    Node b = MRXMLUtils.selectSingleNodeXpath(MRXMLUtils.elementToDocument(policyObjectList.get(i)), xpathList.get(j));

                    if (b == null) {
                        isMatch = false;

                        break;
                    }
                }
            }

            for (int j = 0; isMatch && (xpathArray != null) && (j < xpathArray.length); j++) {
                if ((xpathArray[j] != null) && !("").equals(xpathArray[j].trim())) {
                    Object b = MRXMLUtils.selectSingleNodeXpath(policyObjectList.get(i), xpathArray[j]);

                    if (b == null) {
                        isMatch = false;

                        break;
                    }
                }
            }

            if (isMatch) {
                matchingPolicyObjectList.add(policyObjectList.get(i));
            }
        }

        return matchingPolicyObjectList;
    }

    public static String generateErrorMessage(String tag, String value) {
        return "<error><" + tag + ">" + value + "</" + tag + "></error>";
    }

    /* @SuppressWarnings("unchecked")
     public static List<Element> getMatchingMediaRoomPolicyBasedOnStreamFilterSrcAndDest(List<Element> roomPolicyListElement,
         Element destinationList, String serviceName, MRRequest request, MRDB schema, MRLog log, MRConfig config)
         throws Exception {
         Element srcFilter = null;
         Element destFilter = null;
         List<Element> streamFilterList = getStreamFilterList();
    
         if (streamFilterList == null) {
             String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
             Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
             Element streamFilterListEL = PEMImplementation.getMatchingNodeList(parentNID,
                     PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, responseEL, null, 1, null, null, request, schema, log);
    
             if (streamFilterListEL != null) {
                 streamFilterList = streamFilterListEL.elements();
    
                 if ((streamFilterList != null) && (streamFilterList.size() != 0)) {
                     return null;
                 }
             }
         }
    
         for (Iterator<Element> objIt = roomPolicyListElement.iterator(); objIt.hasNext();) {
                 Element roomPolicyEL = objIt.next();
                 srcFilter =
         }
    
         return null;
     }*/

    /* */
    /**
    * @return the streamFilterList
    */

    /*
    public static List<Element> getStreamFilterList() {
     return streamFilterList;
    }*/

    /**
     * @param streamFilterList the streamFilterList to set
     */

    /*
    public static void setStreamFilterList(List<Element> streamFilterList1) {
     streamFilterList = streamFilterList1;
    }*/

    /*public static Document generateSourceFilterXpathList(List<Element> roomPolicyListElement, Element streamFilterList,
        Element destinationList, Element operatorList, Element roomGroup, String serviceName, MRRequest request, MRDB schema,
        MRLog log) throws Exception {
        List<Element> streamFilterElementList = streamFilterList.elements("MediaStreamFilter");
        List<FilterXpath> roomPolicyXpathList = new ArrayList();
    
        //StringBuffer xpath = new StringBuffer();
        Hashtable<String, Element> filterNIDMapping = new Hashtable<String, Element>();
        Hashtable<String, Element> streamFilterNIDMapping = new Hashtable<String, Element>();
        MRConfig config = MRConfig.getInstance(serviceName, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, log);
        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "List");
        Element filterList = PEMImplementation.getMatchingNodeList(parentNID, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE,
                responseEL, null, 1, null, null, request, schema, log);
    
        //Element filterList = MRXMLDBUtils.findChildElements(parentNID,responseEL, null, 1, null, null, request, schema, log);
        List<Element> filterElement = filterList.elements("TagObjectFilter");
        Element roomGroupList = MRXMLUtils.newElement("RoomList");
        Element room = MRXMLUtils.newElement("Room");
        room.add(roomGroup.detach());
        roomGroupList.add(room);
    
        for (int i = 0; (filterElement != null) && (i < filterElement.size()); i++) {
            filterNIDMapping.put(filterElement.get(i).getAttributeValue("NID"), filterElement.get(i));
        }
    
        for (int i = 0; (streamFilterElementList != null) && (i < streamFilterElementList.size()); i++) {
            streamFilterNIDMapping.put(streamFilterElementList.get(i).getAttributeValue("NID"), streamFilterElementList.get(i));
        }
    
        roomPolicyXpathList = new ArrayList();
    
        if(roomPolicyListElement == null) {
                return MRXMLUtils.stringToDocument("<XPathList></XPathList>");
        }
        for (int i = 0; i < roomPolicyListElement.size(); i++) {
            if (!roomPolicyListElement.get(i).elementText("allow").equalsIgnoreCase("true")) {
                continue;
            }
    
            List<String> userRuleKeyXpath = new ArrayList<String>();
            List<String> resourceRuleKeyXpath = new ArrayList<String>();
            Element roomPolicy = roomPolicyListElement.get(i);
            List<Element> streamPolicyNIDElement = roomPolicy.element("MediaRoomStreamFilterSet")
                                                             .elements("MediaStreamFilterNID");
            @SuppressWarnings("unused")
            List<String> profileRuleKeyXpath = new ArrayList<String>();
    
            if (MediaRoomPolicyHelper.matchRuleKeyTagFilter(roomPolicy, streamFilterNIDMapping, destinationList, operatorList,
                        roomGroupList, userRuleKeyXpath, resourceRuleKeyXpath)) {
                Element sourceUserFilterElement = null;
                Element sourceResourceFilterElement = null;
    
                for (int j = 0; (streamPolicyNIDElement != null) && (j < streamPolicyNIDElement.size()); j++) {
                    Element streamFilterElement = streamFilterNIDMapping.get(streamPolicyNIDElement.get(j).getValue());
    
                    if (streamFilterElement != null) {
                        if (streamFilterElement.element("SrcFilterSet") != null) {
                            List<Element> srcFilterSet = streamFilterElement.element("SrcFilterSet").elements("SrcFilter");
    
                            for (int k = 0; (srcFilterSet != null) && (k < srcFilterSet.size()); k++) {
                                if (srcFilterSet.get(k) != null) {
                                    String sourceUserFilterNID = srcFilterSet.get(k).elementText("UserFilterNID");
                                    String sourceResourceFilterNID = srcFilterSet.get(k)
                                                                                 .elementText("MediaStreamResourceFilterNID");
                                    sourceUserFilterElement = filterNIDMapping.get(sourceUserFilterNID);
                                    sourceResourceFilterElement = filterNIDMapping.get(sourceResourceFilterNID);
    
                                    //TBD :Need to check none condition -- need to conform
                                       if(sourceUserFilterElement==null && sourceResourceFilterElement==null){
                                                throw new MRException(PolicyManagementExceptionCodes.ACCES_DENY_TO_VIEW_SOURCES,
                                            "NO Permission available to view Sources");
                                       }
                                }
                            }
                        }
                    }
                }
    
                roomPolicyXpathList.add(generateSourceFilterXpath(sourceUserFilterElement, sourceResourceFilterElement,
                        userRuleKeyXpath, resourceRuleKeyXpath));
            }
        }
    
        Document xpathDoc = generateXML(roomPolicyXpathList);
    
        //System.out.println(MRXMLUtils.documentToString(xpathDoc));
        return xpathDoc;
    }*/
    @SuppressWarnings("unchecked")
    public static String generateFilterXpath(Element sourceUserFilter) throws MRException,Exception{
        Element tagFilterSetElement = null;

        if (sourceUserFilter != null) {
            tagFilterSetElement = sourceUserFilter.element("TagFilterSet");
        }

        List<Element> tagFilterElement = null;

        if (tagFilterSetElement != null) {
            tagFilterElement = tagFilterSetElement.elements("TagFilter");
        }

        StringBuffer userXpath = null;

        for (int i = 0; (tagFilterElement != null) && (i < tagFilterElement.size()); i++) {
            if ((tagFilterElement.get(i) == null) || "".equals(tagFilterElement.get(i).elementText("LHTagName").trim()) || "".equals(tagFilterElement.get(i).elementText("MatchOp").trim())) {
                continue;
            }

            if (userXpath == null) {
                userXpath = new StringBuffer();
            } else {
                userXpath.append(" and ");
            }

            String tagName = tagFilterElement.get(i).elementText("LHTagName");
            String matchOp = tagFilterElement.get(i).elementText("MatchOp");
            String regExp = tagFilterElement.get(i).elementText("RHRegexp");
            String xpath = null;

            xpath = "(" + tagName +  getMatchOperator(matchOp) + "'" + regExp + "')";            

            if (xpath != null) {
                userXpath.append(xpath);
            }
        }

        if ((userXpath == null) || userXpath.toString().isEmpty()) {
            userXpath = new StringBuffer();
        } else {
            userXpath = new StringBuffer("(" + userXpath.toString() + ")");
        }

        return userXpath.toString();
    }

    /*@SuppressWarnings("unchecked")
    public static boolean checkFilterType(Element filter, String type) {
        if ((filter != null) && (type != null) && (filter.element("TagObjectTypeList") != null)) {
            List<Element> filterTypeList = filter.element("TagObjectTypeList").elements("TagObjectType");
    
            for (int i = 0; (filterTypeList != null) && (i < filterTypeList.size()); i++) {
                if (filterTypeList.get(i).getText() != null) {
                    if (filterTypeList.get(i).getText().trim().equalsIgnoreCase(type.trim())) {
                        return true;
                    }
                }
            }
        } else {
            return false;
        }
    
        return false;
    }*/

    //common method to get the src/dest xpath from given src/dst list -- added by padmaja
    public static List<String> getSrcDestXPath(List<Element> srcDestElement, boolean allCheck, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        Element[][] destinationFilterElement = null;

        //TBD
        if (allCheck) {
            destinationFilterElement = findMatchingDestinationGroupFilter_New(srcDestElement, ServiceClient.POLICYADMINSERVICE, request, db, log);
        } else {
            destinationFilterElement = findMatchingDestinationGroupFilter_New(srcDestElement, ServiceClient.POLICYADMINSERVICE, false, request, db, log);
        }

        List<?> srcDestUserFilterList = null;
        List<?> srcDestResourceFilterList = null;

        List<String> srcDestXpath = new ArrayList<String>();

        for (int i = 0; (i < 30) && (srcDestElement != null) && (destinationFilterElement != null); i++) {
            if (destinationFilterElement[i][0] == null) {
                break;
            }

            srcDestUserFilterList = destinationFilterElement[i][0].elements("TagObjectFilter");
            srcDestResourceFilterList = destinationFilterElement[i][1].elements("TagObjectFilter");

            List<String> destinationUserGroupNidList = new ArrayList<String>();
            List<String> destinationResourceGroupNidList = new ArrayList<String>();

            for (int j = 0; (srcDestUserFilterList != null) && (j < srcDestUserFilterList.size()); j++) {
                destinationUserGroupNidList.add(MRXMLUtils.getAttribute(((Element) srcDestUserFilterList.get(j)),"NID").getValue());
            }

            for (int j = 0; (srcDestResourceFilterList != null) && (j < srcDestResourceFilterList.size()); j++) {
                destinationResourceGroupNidList.add(MRXMLUtils.getAttribute(((Element) srcDestResourceFilterList.get(j)),"NID").getValue());
            }

            srcDestXpath.add(getXPathforSrcDstFilter(destinationUserGroupNidList, destinationResourceGroupNidList));
        }

        return srcDestXpath;
    }

    /*************************************************/
}


class CaseFilter {
    String filter1 = null;
    String filter2 = null;

    public CaseFilter(String filter1, String filter2) {
        super();
        this.filter1 = filter1;
        this.filter2 = filter2;
    }

    @Override
    public boolean equals(Object obj) {
        // TODO Auto-generated method stub
        if ((obj != null) && !obj.getClass().equals(this.getClass())) {
            return false;
        }

        CaseFilter filter2 = (CaseFilter) obj;

        if ((this.filter1 == null) || (this.filter2 == null) || (filter2.filter1 == null) || (filter2.filter2 == null)) {
            return false;
        }

        if ((this.filter1.equals(filter2.filter1) && this.filter2.equals(filter2.filter2)) || (this.filter1.equals(filter2.filter2) && this.filter2.equals(filter2.filter1))) {
            return true;
        } else {
            return false;
        }
    }

    @Override
    public int hashCode() {
        // TODO Auto-generated method stub
        return 0;
    }
}
