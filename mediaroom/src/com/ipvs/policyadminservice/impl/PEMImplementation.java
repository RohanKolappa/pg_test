package com.ipvs.policyadminservice.impl;

import java.util.ArrayList;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Element;
import org.dom4j.Attribute;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.handler.MediaRoomPolicyHelper;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;


public class PEMImplementation {
    //get the list of nodes 
    //not used any more
    /*public static List<String> getPolicyList(String objectType, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        MRConfig config = null;
        config = MRConfig.getInstance(serviceName, objectType, log);

        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        List<String> deviceList = schema.elements(request, parentNID);

        return deviceList;
    }*/

    // get the matching nodes
    public static Element getMatchingNodeList(String parentNID, String objectType, Element responceEL, Element operatonData, int whereSelectNodeDepth, Element whereAssistXML, String[] whereXPath,
        MRRequest request, MRDB schema, MRLog log) throws MRException, Exception {
        //PEMDBUtils objPEM = PEMDBUtils.getInstatnce();

        //responceEL = objPEM.getMatchingNodes(parentNID, objectType, whereXPath, request, schema, log);
    	responceEL = PolicyAdminAgent.getMatchingNodes(parentNID, objectType, whereXPath, request, schema, log);

        if ((responceEL != null)) {
            return responceEL;
        }

        return null;
    }

    //Get the operator filter list by giving input as Operator list
    @SuppressWarnings("unchecked")
    public static OperatorFilterBean getOperatorFilterList_New(Element destination, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        OperatorFilterBean objOperatorBean = null;
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
        List<String> filterXpathList = MediaRoomPolicyHelper.generateXPathforTagFilter(filterList);

        if (filterXpathList == null) { //No filter found in database 

            return null;
        }

        List<Element> userList = destination.elements("User");
        List<Element> resourceList = destination.elements("AssetResource");
        List<Element> userFilterList = new ArrayList<Element>();
        List<Element> resourceFilterList = new ArrayList<Element>();

        for (int j = 0; j < filterXpathList.size(); j++) {
            List<Element> tempUserList = null;
            List<Element> tempResourceList = null;
            List<String> xpath = new ArrayList<String>();

            if ((filterXpathList.get(j) != null) && (filterXpathList.get(j).length() > 0)) {
                xpath.add("//" + "User" + "[" + filterXpathList.get(j) + "]");
            } else {
                xpath.add(filterXpathList.get(j));
            }

            tempUserList = MediaRoomPolicyHelper.matchXPath(userList, xpath, null);
            xpath = new ArrayList<String>();

            if ((filterXpathList.get(j) != null) && (filterXpathList.get(j).length() > 0)) {
                xpath.add("//" + "AssetResource" + "[" + filterXpathList.get(j) + "]");
            } else {
                xpath.add(filterXpathList.get(j));
            }

            tempResourceList = MediaRoomPolicyHelper.matchXPath(resourceList, xpath, null);

            if ((tempUserList != null) && (tempUserList.size() > 0)) {
                userFilterList.add((Element) filterList.get(j).clone());
            }

            if ((tempResourceList != null) && (tempResourceList.size() > 0)) {
                resourceFilterList.add((Element) filterList.get(j).clone());
            }
        }

        if ((userFilterList != null) && (userFilterList.size() > 0) && (resourceFilterList != null) && (resourceFilterList.size() > 0)) {
            //Element userFilter = MediaRoomPolicyHelper.generateDocumentFromList(userFilterList);
            //Element resourceFilter = MediaRoomPolicyHelper.generateDocumentFromList(resourceFilterList);
            List<String> userFilterStringList = new ArrayList<String>();
            List<String> resourceFilterStringList = new ArrayList<String>();

            for (int k = 0; k < userFilterList.size(); k++) {
                userFilterStringList.add(MRXMLUtils.getAttributeValue(userFilterList.get(k),PolicyAdminConstants.NID));
            }

            for (int k = 0; k < resourceFilterList.size(); k++) {
                resourceFilterStringList.add(MRXMLUtils.getAttributeValue(resourceFilterList.get(k),PolicyAdminConstants.NID));
            }

            objOperatorBean = new OperatorFilterBean();
            objOperatorBean.setResourceFilterList(resourceFilterStringList);
            objOperatorBean.setUserFilterList(userFilterStringList);
        }

        return objOperatorBean;
    }

    public static OperatorFilterBean getOperatorFilterList(Element operatorEL, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        List<?> groupsList = null;
        OperatorFilterBean objOperatorBean = null;
        StringBuffer whereXPath = new StringBuffer();
        objOperatorBean = new OperatorFilterBean();

        List<String> filterNIDList = null;

        //System.out.println(MRXMLUtils.elementToString(operatorEL));
        Element userGroups = operatorEL.element("User");
        groupsList = userGroups.elements("Tag"); // MRXMLUtils.getList(operatorEL, "UserGroups/Tag");
                                                    //System.out.println("GroupsList %%%%%%% " + groupsList);

        StringBuffer xpath = null;
        whereXPath.append("//" + PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "[");

        if ((groupsList != null)) {
            Element userGroup = null;
            whereXPath.append(" ( ");

            xpath = null;

            for (Iterator<?> userGroupsIT = groupsList.iterator(); userGroupsIT.hasNext();) {
                userGroup = (Element) userGroupsIT.next();

                if (xpath == null) {
                    xpath = new StringBuffer();
                }

                xpath.append(" (  ( " +
                    MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_NAME,
                        MRXMLUtils.getAttributeValue(userGroup, "Name", "").trim()));
                xpath.append(" and ");

                xpath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.MATCH_OPERATOR,
                        PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO));

                xpath.append(" and ");

                xpath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_RHREGEXP,
                        MRXMLUtils.getAttributeValue(userGroup, "Value", "").trim()) + " ) ");

                xpath.append(" or ");

                xpath.append("   ( " +
                    MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_NAME,
                        MRXMLUtils.getAttributeValue(userGroup, "Name", "").trim()));
                xpath.append(" and ");

                xpath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.MATCH_OPERATOR,
                        PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO));

                xpath.append(" and ");

                xpath.append(" ( " + PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_RHREGEXP + " != '" +
                    MRXMLUtils.getAttributeValue(userGroup, "Value", "").trim() + " ' ) ) ) ");

                if (userGroupsIT.hasNext()) {
                    xpath.append(" or ");
                }
            }

            if ((xpath != null) && ((xpath.toString().trim()).length() != 0)) {
                whereXPath.append(xpath.toString() + " or " + MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_NAME, "_ALL_") + ") ");
            } else {
                whereXPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_NAME, "_ALL_") + ") ");
            }

            whereXPath.append("  and  " + MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_OBJECT_TYPE_LIST + "/" + PolicyAdminConstants.TAG_OBJECT_TYPE, PolicyAdminConstants.USER_OBJECT) + " ]");

            if ((whereXPath != null) && (whereXPath.length() != 0)) {
                Element filterListEL = getMatchingNodes(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, whereXPath.toString(), serviceName, request, schema, log);

                if (filterListEL != null) {
                    filterNIDList = getNIDList(filterListEL, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE);

                    if ((filterNIDList != null) && (filterNIDList.size() != 0)) {
                        objOperatorBean.setUserFilterList(filterNIDList);
                    } else {
                        return null;
                    }

                    //System.out.println("user filter nid list %%%%%%% "+filterNIDList);
                } else {
                    return null;
                }
            }

            if ((filterNIDList != null) && (filterNIDList.size() != 0)) {
                Element resourcegroups = operatorEL.element("ResourceGroups");
                groupsList = resourcegroups.elements("Tag");
                //groupsList = MRXMLUtils.getListXpath(operatorEL, "//ResourceGroups/Tag");
                whereXPath = new StringBuffer("");
                whereXPath.append("//" + PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE + "[");

                List<String> resourceList;

                if (groupsList != null) {
                    Element resourceGroup = null;
                    xpath = null;
                    whereXPath.append(" ( ");

                    for (Iterator<?> resourceGroupsIT = groupsList.iterator(); resourceGroupsIT.hasNext();) {
                        if (xpath == null) {
                            xpath = new StringBuffer();
                        }

                        resourceGroup = (Element) resourceGroupsIT.next();
                        xpath.append(" ( ( " +
                            MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_NAME,
                                MRXMLUtils.getAttributeValue(resourceGroup, "Name", "").trim()));
                        xpath.append(" and ");
                        xpath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.MATCH_OPERATOR,
                                PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO));
                        xpath.append(" and ");
                        xpath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_RHREGEXP,
                                MRXMLUtils.getAttributeValue(resourceGroup, "Value", "").trim()) + " ) ");
                        xpath.append(" or ");
                        xpath.append(" ( " +
                            MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_NAME,
                                MRXMLUtils.getAttributeValue(resourceGroup, "Name", "").trim()));
                        xpath.append(" and ");
                        xpath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.MATCH_OPERATOR,
                                PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO));
                        xpath.append(" and ");
                        xpath.append(" ( " + PolicyAdminConstants.TAG_FILTER_SET + "/" + PolicyAdminConstants.TAG_FILTER + "/" + PolicyAdminConstants.FILTER_TAG_RHREGEXP + " != '" +
                            MRXMLUtils.getAttributeValue(resourceGroup, "Value", "").trim() + "' ) )) ");

                        if (resourceGroupsIT.hasNext()) {
                            xpath.append(" or ");
                        }
                    }

                    if ((xpath != null) && ((xpath.toString().trim()).length() != 0)) {
                        whereXPath.append(xpath.toString() + " or " + MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_NAME, "_ALL_") + ") ");
                    } else {
                        whereXPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_FILTER_NAME, "_ALL_") + ") ");
                    }

                    whereXPath.append("  and  " +
                        MRXMLUtils.buildXPath(PolicyAdminConstants.TAG_OBJECT_TYPE_LIST + "/" + PolicyAdminConstants.TAG_OBJECT_TYPE, PolicyAdminConstants.ASSET_RESOURCE_OBJECT) + " ]");

                    //System.out.println("TAg object XPATh %%%%%%% "+whereXPath.toString());
                    if ((whereXPath != null) && (whereXPath.length() != 0)) {
                        Element filterListEL = getMatchingNodes(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, whereXPath.toString(), serviceName, request, schema, log);

                        if (filterListEL != null) {
                            resourceList = getNIDList(filterListEL, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE);

                            if ((resourceList != null) && (resourceList.size() != 0)) {
                                objOperatorBean.setResourceFilterList(resourceList);
                            } else {
                                return null;
                            }

                            //System.out.println(" resource nid list %%%%%%%      "+resourceList);
                        } else {
                            return null;
                        }
                    }
                }
            }
        }

        return objOperatorBean;
    }

    //Get the matching policies list by giving Filter List, Object type and Role action
    private static List<?> getMatchingPolicies(List<OperatorFilterBean> objOperatorFilterList, String policyObjectType, String action, String roleObjectType, String serviceName, MRRequest request,
        MRDB schema, MRLog log) throws MRException, Exception {
        StringBuffer whereXPath = new StringBuffer("");
        OperatorFilterBean objOperatorBean = null;
        List<String> userFilterNIDList = null;
        List<String> resourceFilterNIDList = null;
        String validXPath = MediaRoomPolicyHelper.buildXPath(policyObjectType, MRXMLUtils.buildXPath("valid", "true"));

        for (Iterator<OperatorFilterBean> objIT = objOperatorFilterList.iterator(); objIT.hasNext();) {
            //whereXPath.append(" ( ");
            whereXPath.append(" ( ");
            objOperatorBean =  objIT.next();
            userFilterNIDList = objOperatorBean.getUserFilterList();

            if ((userFilterNIDList != null) && (userFilterNIDList.size() != 0)) {
                for (Iterator<String> objUserNID = userFilterNIDList.iterator(); objUserNID.hasNext();) {
                    whereXPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.USER_FILTER_NID, objUserNID.next()));

                    if (objUserNID.hasNext()) {
                        whereXPath.append(" or ");
                    }
                }
            } else {
                whereXPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.USER_FILTER_NID, ""));
            }

            whereXPath.append(" ) and (");

            resourceFilterNIDList = objOperatorBean.getResourceFilterList();

            if ((resourceFilterNIDList != null) && (resourceFilterNIDList.size() != 0)) {
                for (Iterator<String> objResourceNIDIT = resourceFilterNIDList.iterator(); objResourceNIDIT.hasNext();) {
                    whereXPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.RESOURCE_FILTER_NID, objResourceNIDIT.next()));

                    if (objResourceNIDIT.hasNext()) {
                        whereXPath.append(" or ");
                    }
                }
            } else {
                whereXPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.RESOURCE_FILTER_NID, ""));
            }

            if (objIT.hasNext()) {
                whereXPath.append(" ) or (");
            }
        }

        whereXPath.append(") ");

        String parentTagXPath = policyObjectType + "/" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER_SET + "/" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER;
        String[] xPathArray = new String[3];
        xPathArray[0] = validXPath;

        String filterXPath = MediaRoomPolicyHelper.buildXPath(parentTagXPath, whereXPath.toString());
        xPathArray[1] = filterXPath;

        String roleXPath = MediaRoomPolicyHelper.buildXPath(policyObjectType, getRoleXPath(policyObjectType, action, roleObjectType));
        xPathArray[2] = roleXPath;

        Element policyListEL = getMatchingNodes(request, schema, log, policyObjectType, xPathArray, serviceName);
        List<?> policLIst = MRXMLUtils.getList(policyListEL, policyObjectType);

        return policLIst;
    }

    private static String getRoleXPath(String policyObjectType, String action, String roleObjectType) {
        if (policyObjectType.equals(PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE)) {
            return getAssetAdminPolicyRoleXPath(action, roleObjectType);
        } else if (policyObjectType.equals(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE)) {
            return getMediaRoomPolicyRoleXPath(action, roleObjectType);
        } else if (policyObjectType.equals(PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE)) {
            return getMediaStoreAdminPolicyRoleXPath(action, roleObjectType);
        } else if (policyObjectType.equals(PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE)) {
            return getPolicyAdminPolicyRoleXPath(action, roleObjectType);
        } else if (policyObjectType.equals(PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE)) {
            return getUserAdminPolicyRoleXPath(action, roleObjectType);
        }

        return null;
    }

    //return role xpath for user admin policy
    private static String getUserAdminPolicyRoleXPath(String action, String roleObjectType) {
        StringBuffer xPath = new StringBuffer("");
        xPath.append(" ( ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.USER_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.USER_ADMIN_OBJECT_ROLE + "/" + PolicyAdminConstants.ROLE_OBJECT, roleObjectType));
        xPath.append(" and ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.USER_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.USER_ADMIN_OBJECT_ROLE + "/" + PolicyAdminConstants.ROLE_ACTION, action));
        xPath.append(" ) ");

        return xPath.toString();
    }

    //return role xpath for policy admin policy
    private static String getPolicyAdminPolicyRoleXPath(String action, String roleObjectType) {
        StringBuffer xPath = new StringBuffer("");
        xPath.append(" ( ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.POLICY_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.POLICY_ADMIN_OBJECT_ROLE + "/" + PolicyAdminConstants.ROLE_OBJECT, roleObjectType));
        xPath.append(" and ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.POLICY_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.POLICY_ADMIN_OBJECT_ROLE + "/" + PolicyAdminConstants.ROLE_ACTION, action));
        xPath.append(" ) ");

        return xPath.toString();
    }

    //return role xpath for media store admin policy
    private static String getMediaStoreAdminPolicyRoleXPath(String action, String roleObjectType) {
        StringBuffer xPath = new StringBuffer("");
        xPath.append(" ( ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.MEDIA_STORE_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.MEDIA_STORE_ADMIN_OBJECT_ROLE + "/" + PolicyAdminConstants.ROLE_OBJECT,
                roleObjectType));
        xPath.append(" and ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.MEDIA_STORE_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.MEDIA_STORE_ADMIN_OBJECT_ROLE + "/" + PolicyAdminConstants.ROLE_ACTION, action));
        xPath.append(" ) ");

        return xPath.toString();
    }

    // return role xpath for Media Room Policy
    private static String getMediaRoomPolicyRoleXPath(String action, String roleObjectType) {
        StringBuffer xPath = new StringBuffer("");
        xPath.append(" ( ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.MEDIA_ROOM_ROLE_SET + "/" + PolicyAdminConstants.MEDIA_ROOM_ROLE + "/" + PolicyAdminConstants.ROLE_OBJECT, roleObjectType));
        xPath.append(" and ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.MEDIA_ROOM_ROLE_SET + "/" + PolicyAdminConstants.MEDIA_ROOM_ROLE + "/" + PolicyAdminConstants.ROLE_ACTION, action));
        xPath.append(" ) ");

        return xPath.toString();
    }

    //return role xpath for asset admin policy
    private static String getAssetAdminPolicyRoleXPath(String action, String roleObjectType) {
        StringBuffer xPath = new StringBuffer("");
        xPath.append(" ( ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.ASSET_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.ASSET_ADMIN_ROLE + "/" + PolicyAdminConstants.ROLE_OBJECT, roleObjectType));
        xPath.append(" and ");
        xPath.append(MRXMLUtils.buildXPath(PolicyAdminConstants.ASSET_ADMIN_ROLE_SET + "/" + PolicyAdminConstants.ASSET_ADMIN_ROLE + "/" + PolicyAdminConstants.ROLE_ACTION, action));
        xPath.append(" ) ");

        return xPath.toString();
    }

    //Get the list of matching polices based on Operator list and role
    public static List<?> getMatchingPoliciesListBasedOnOperatorAndRole(List<?> operatorList, String policyObjectType, String action, String roleObjectType, String serviceName, MRRequest request,
        MRDB schema, MRLog log) throws MRException, Exception {
        List<?> matchingPolicyList = null;

        List<OperatorFilterBean> filterList = new ArrayList<OperatorFilterBean>();
        int countCheck = 0;

        for (Iterator<?> operatorIT = operatorList.iterator(); operatorIT.hasNext(); countCheck++) {
            Element operatorEL = (Element) operatorIT.next();

            OperatorFilterBean obj = getOperatorFilterList_New(operatorEL, serviceName, request, schema, log);

            if (obj == null) {
                return null;
            }

            filterList.add(obj);
        }

        matchingPolicyList = getMatchingPolicies(filterList, policyObjectType, action, roleObjectType, serviceName, request, schema, log);

        return matchingPolicyList;
    }

    //get the nid list based on given object type and nodes list
    public static List<String> getNIDList(Element nodeListEL, String objectType)
        throws MRException, Exception {
        List<?> objectELList = MRXMLUtils.getList(nodeListEL, objectType);

        List<String> nidList = new ArrayList<String>();
        String nid = null;

        for (Iterator<?> objIT = objectELList.iterator(); objIT.hasNext();) {
            Element nodeEL = (Element) objIT.next();

            nid = MRXMLUtils.getAttributeValue(nodeEL, PolicyAdminConstants.NID, "");

            nidList.add(nid);
        }

        return nidList;
    }

    private static Element getMatchingNodes(MRRequest request, MRDB schema, MRLog log, String objectType, String[] xpath, String serviceName)
        throws MRException, Exception {
        MRConfig config = null;
        config = MRConfig.getInstance(serviceName, objectType, log);

        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element whereAssistXML = MRXMLUtils.newElement(PolicyAdminConstants.WHERE_ASSIST_XML);
        Element childEL = MRXMLUtils.newElement(objectType + "List");
        String[] arrXpath = xpath;

        if (xpath == null) {
            arrXpath = null;
            whereAssistXML = null;
        }

        int whereSelectNodeDepth = 1;
        Element objectList = getMatchingNodeList(parentNID, objectType, childEL, null, whereSelectNodeDepth, whereAssistXML, arrXpath, request, schema, log);

        if ((objectList != null)) {
            return objectList;
        }

        return null;
    }

    private static Element getMatchingNodes(String objectType, String xpath, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        MRConfig config = null;
        config = MRConfig.getInstance(serviceName, objectType, log);

        String parentNID = schema.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element whereAssistXML = MRXMLUtils.newElement(PolicyAdminConstants.WHERE_ASSIST_XML);
        Element childEL = MRXMLUtils.newElement(objectType + "List");
        String[] arrXpath = { xpath };

        if (xpath == null) {
            arrXpath = null;
            whereAssistXML = null;
        }

        int whereSelectNodeDepth = 1;
        Element objectList = getMatchingNodeList(parentNID, objectType, childEL, null, whereSelectNodeDepth, whereAssistXML, arrXpath, request, schema, log);

        if ((objectList != null)) {
            return objectList;
        }

        return null;
    }

    private static int _oldEvaluateRuleKey(List<?> ruleKeyFilteList, List<StringBuffer> targetXPathList, List<?> operatorList)
    throws MRException, Exception {
    String LHRuleKey = null;
    String RHRuleKey = null;
    String LHTagName = null;
    String RHTagName = null;
    String matchOp = null;
    Element ruleKeyEl = null;
    boolean operatorFlag = false;
    int countCheck = 0;

    List<?> userGroupsList = null;
    List<?> resourceGroupsList = null;
    Element operatorEL = null;
    Element userGroupEL = null;
    Element resourceGroupEL = null;
    Element userGroupsEL = null;
    Element resourceGroupsEL = null;

    String userTagName = null;
    String resourceTagName = null;
    String userTagValue = null;
    String resourceTagValue = null;
    StringBuffer xpath = null;

    for (Iterator<?> objRuleKeyIT = ruleKeyFilteList.iterator(); objRuleKeyIT.hasNext();) {
        operatorFlag = false;
        ruleKeyEl = (Element) objRuleKeyIT.next();

        Element objRuleEL = MRXMLUtils.getElement(ruleKeyEl, PolicyAdminConstants.RULE_KEY_LH_TAG_NAME);

        LHRuleKey = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.RULE_KEY, "");

        LHTagName = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.TAG_NAME, "");

        objRuleEL = MRXMLUtils.getElement(ruleKeyEl, PolicyAdminConstants.RULE_KEY_RH_TAG_NAME);
        RHRuleKey = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.RULE_KEY, "");

        RHTagName = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.TAG_NAME, "");

        matchOp = MRXMLUtils.getValue(ruleKeyEl, PolicyAdminConstants.MATCH_OPERATOR, "");

        if (((LHRuleKey == null) || (LHRuleKey.length() == 0)) && ((RHRuleKey == null) || (RHRuleKey.length() == 0))) {
            operatorFlag = true;
        } else {
            if (LHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        userGroupsEL = operatorEL.element("UserGroup");
                        userGroupsList = userGroupsEL.elements("Tag");
                        resourceGroupsEL = operatorEL.element("ResourceGroups");
                        resourceGroupsList = resourceGroupsEL.elements("Tag");

                        //userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//UserGroups/Tag");
                        //resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//ResourceGroups/Tag");
                        for (int i = 0; i < userGroupsList.size(); i++) {
                            userGroupEL = (Element) userGroupsList.get(i);
                            userTagName = MRXMLUtils.getAttributeValue(userGroupEL, "Name", "");
                            userTagValue = MRXMLUtils.getAttributeValue(userGroupEL, "Value", "");

                            for (int j = 0; j < resourceGroupsList.size(); j++) {
                                resourceGroupEL = (Element) resourceGroupsList.get(j);
                                resourceTagName = MRXMLUtils.getAttributeValue(resourceGroupEL, "Name", "");
                                resourceTagValue = MRXMLUtils.getAttributeValue(resourceGroupEL, "Value", "");

                                if (userTagName.equals(LHTagName) && resourceTagName.equals(RHTagName)) {
                                    if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO)) {
                                        if (userTagValue.equals(resourceTagValue)) {
                                            operatorFlag = true;

                                            break;
                                        }
                                    } else if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO)) {
                                        if (!userTagValue.equals(resourceTagValue)) {
                                            operatorFlag = true;

                                            break;
                                        }
                                    }
                                }
                            }

                            if (operatorFlag) {
                                break;
                            }
                        }

                        if (operatorFlag) {
                            break;
                        }
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        //userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//UserGroups/Tag");
                        userGroupsEL = operatorEL.element("UserGroup");
                        userGroupsList = userGroupsEL.elements("Tag");

                        for (int i = 0; i < userGroupsList.size(); i++) {
                            userGroupEL = (Element) userGroupsList.get(i);
                            userTagName = MRXMLUtils.getAttributeValue(userGroupEL, "Name", "");
                            userTagValue = MRXMLUtils.getAttributeValue(userGroupEL, "Value", "");

                            if (userTagName.equals(LHTagName)) {
                                operatorFlag = true;
                                xpath.append(" ( ");

                                if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", RHTagName));
                                    xpath.append(" and ");
                                    xpath.append(MRXMLUtils.buildXPath("@Value", userTagValue));
                                } else if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", RHTagName));
                                    xpath.append(" and ");
                                    xpath.append(" ( @Value !=" + userTagValue + " ) ");
                                }

                                xpath.append(" ) ");

                                if (i < (userGroupsList.size() - 1)) {
                                    xpath.append(" or ");
                                }
                            }
                        }
                    }
                }

                if ((xpath != null) && (xpath.length() != 0)) {
                    targetXPathList.add(xpath);
                }
            } else if (LHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        // userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//UserGroups/Tag");
                        // resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//ResourceGroups/Tag");
                        userGroupsEL = operatorEL.element("UserGroup");
                        userGroupsList = userGroupsEL.elements("Tag");
                        resourceGroupsEL = operatorEL.element("ResourceGroups");
                        resourceGroupsList = resourceGroupsEL.elements("Tag");

                        for (int i = 0; i < resourceGroupsList.size(); i++) {
                            resourceGroupEL = (Element) resourceGroupsList.get(i);
                            resourceTagName = MRXMLUtils.getAttributeValue(resourceGroupEL, "Name", "");
                            resourceTagValue = MRXMLUtils.getAttributeValue(resourceGroupEL, "Value", "");

                            for (int j = 0; j < resourceGroupsList.size(); j++) {
                                userGroupEL = (Element) userGroupsList.get(j);
                                userTagName = MRXMLUtils.getAttributeValue(userGroupEL, "Name", "");
                                userTagValue = MRXMLUtils.getAttributeValue(userGroupEL, "Value", "");

                                if (userTagName.equals(RHTagName) && resourceTagName.equals(LHTagName)) {
                                    if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO)) {
                                        if (userTagValue.equals(resourceTagValue)) {
                                            operatorFlag = true;

                                            break;
                                        }
                                    } else if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO)) {
                                        if (!userTagValue.equals(resourceTagValue)) {
                                            operatorFlag = true;

                                            break;
                                        }
                                    }
                                }
                            }

                            if (operatorFlag) {
                                break;
                            }
                        }

                        if (operatorFlag) {
                            break;
                        }
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        resourceGroupsEL = operatorEL.element("ResourceGroups");
                        resourceGroupsList = resourceGroupsEL.elements("Tag");

                        //resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//ResourceGroups/Tag");
                        for (int i = 0; i < resourceGroupsList.size(); i++) {
                            resourceGroupEL = (Element) resourceGroupsList.get(i);
                            resourceTagName = MRXMLUtils.getAttributeValue(resourceGroupEL, "Name", "");
                            resourceTagValue = MRXMLUtils.getAttributeValue(resourceGroupEL, "Value", "");

                            if (resourceTagName.equals(LHTagName)) {
                                operatorFlag = true;
                                xpath.append(" ( ");

                                if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", RHTagName));
                                    xpath.append(" and ");
                                    xpath.append(MRXMLUtils.buildXPath("@Value", resourceTagValue));
                                } else if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", RHTagName));
                                    xpath.append(" and ");
                                    xpath.append(" ( @Value !=" + resourceTagValue + " ) ");
                                }

                                xpath.append(" ) ");

                                if (i < (resourceGroupsList.size() - 1)) {
                                    xpath.append(" or ");
                                }
                            }
                        }
                    }
                }

                if ((xpath != null) && (xpath.length() != 0)) {
                    targetXPathList.add(xpath);
                }
            } else if (LHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        userGroupsEL = operatorEL.element("UserGroup");
                        userGroupsList = userGroupsEL.elements("Tag");

                        //  userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//UserGroups/Tag");
                        for (int i = 0; i < userGroupsList.size(); i++) {
                            userGroupEL = (Element) userGroupsList.get(i);
                            userTagName = MRXMLUtils.getAttributeValue(userGroupEL, "Name", "");
                            userTagValue = MRXMLUtils.getAttributeValue(userGroupEL, "Value", "");

                            if (userTagName.equals(RHTagName)) {
                                operatorFlag = true;
                                xpath.append(" ( ");

                                if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", LHTagName));
                                    xpath.append(" and ");
                                    xpath.append(MRXMLUtils.buildXPath("@Value", userTagValue));
                                } else if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", LHTagName));
                                    xpath.append(" and ");
                                    xpath.append(" ( @Value !=" + userTagValue + " ) ");
                                }

                                xpath.append(" ) ");

                                if (i < (userGroupsList.size() - 1)) {
                                    xpath.append(" or ");
                                }
                            }
                        }
                    }

                    if ((xpath != null) && (xpath.length() != 0)) {
                        targetXPathList.add(xpath);
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        resourceGroupsEL = operatorEL.element("ResourceGroups");
                        resourceGroupsList = resourceGroupsEL.elements("Tag");

                        //resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//ResourceGroups/Tag");
                        for (int i = 0; i < resourceGroupsList.size(); i++) {
                            resourceGroupEL = (Element) resourceGroupsList.get(i);
                            resourceTagName = MRXMLUtils.getAttributeValue(resourceGroupEL, "Name", "");
                            resourceTagValue = MRXMLUtils.getAttributeValue(resourceGroupEL, "Value", "");

                            if (resourceTagName.equals(RHTagName)) {
                                operatorFlag = true;
                                xpath.append(" ( ");

                                if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", LHTagName));
                                    xpath.append(" and ");
                                    xpath.append(MRXMLUtils.buildXPath("@Value", resourceTagValue));
                                } else if (matchOp.equals(PolicyAdminConstants.MATCH_OPERATOR_NOT_EQUAL_TO)) {
                                    xpath.append(MRXMLUtils.buildXPath("@Name", LHTagName));
                                    xpath.append(" and ");
                                    xpath.append(" ( @Value !=" + resourceTagValue + " ) ");
                                }

                                xpath.append(" ) ");

                                if (i < (resourceGroupsList.size() - 1)) {
                                    xpath.append("  or ");
                                }
                            }
                        }
                    }

                    if ((xpath != null) && (xpath.length() != 0)) {
                        targetXPathList.add(xpath);
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                }
            }
        }

        if (operatorFlag) {
            countCheck++;
        }
    }

    return countCheck;
}
    
    private static int evaluateRuleKey(List<?> ruleKeyFilteList, List<StringBuffer> targetXPathList, List<?> operatorList)
    throws MRException, Exception {
    String LHRuleKey = null;
    String RHRuleKey = null;
    String LHTagName = null;
    String RHTagName = null;
    String matchOp = null;
    Element ruleKeyEl = null;
    boolean operatorFlag = false;
    int countCheck = 0;

    List<?> userGroupsList = null;
    List<?> resourceGroupsList = null;
    Element operatorEL = null;
    Attribute attr = null;
    Attribute userAttr = null;
    Attribute resourceAttr = null;
    
    String _xpath = null;
    StringBuffer xpath = null;

    for (Iterator<?> objRuleKeyIT = ruleKeyFilteList.iterator(); objRuleKeyIT.hasNext();) {
        operatorFlag = false;
        ruleKeyEl = (Element) objRuleKeyIT.next();

        Element objRuleEL = MRXMLUtils.getElement(ruleKeyEl, PolicyAdminConstants.RULE_KEY_LH_TAG_NAME); 

        LHRuleKey = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.RULE_KEY, "");

        LHTagName = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.TAG_NAME, "");

        objRuleEL = MRXMLUtils.getElement(ruleKeyEl, PolicyAdminConstants.RULE_KEY_RH_TAG_NAME);
     
        RHRuleKey = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.RULE_KEY, "");

        RHTagName = MRXMLUtils.getValue(objRuleEL, PolicyAdminConstants.TAG_NAME, "");

        matchOp = MRXMLUtils.getValue(ruleKeyEl, PolicyAdminConstants.MATCH_OPERATOR, "");

        PEMImplementation pemImpl = new PEMImplementation();
        
        if (((LHRuleKey == null) || (LHRuleKey.length() == 0)) && ((RHRuleKey == null) || (RHRuleKey.length() == 0))) {
            operatorFlag = true;
        } else {
            if (LHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();                
                        
                        _xpath = pemImpl.generateXPath(RHTagName);
                        
                        userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//User" + _xpath);
                        resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//AssetResource" + _xpath);
                                                
                        for (int i = 0; i < userGroupsList.size(); i++) {                                                    
                        	userAttr = (Attribute) userGroupsList.get(i);                                                                                                                

                            for (int j = 0; j < resourceGroupsList.size(); j++) {                                
                            	resourceAttr = (Attribute) resourceGroupsList.get(j);                                                       
                                                            	
                                if (matchOp.equals("=")) {
                                	if (userAttr.getText().equals(resourceAttr.getText())) {
                                		operatorFlag = true;
                                        break;
                                    }
                                } else if (matchOp.equals("!=")) {
                                	if (!userAttr.getText().equals(resourceAttr.getText())) {
                                		operatorFlag = true;
                                        break;
                                    }
                                }                                
                            }

                            if (operatorFlag) {
                                break;
                            }
                        }

                        if (operatorFlag) {
                            break;
                        }
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();

                        _xpath = pemImpl.generateXPath(RHTagName);
                        
						userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//User" + _xpath);
						for (int i = 0; i < userGroupsList.size(); i++) {
                             attr = (Attribute) userGroupsList.get(i);                                                       
                             operatorFlag = true;                                       
                             xpath.append(" ( (" + RHTagName + matchOp + "'" + attr.getText() + "') )");                                                             
                             if (i < (userGroupsList.size() - 1)) {
                            	 xpath.append(" or ");
                             }                              
                        }                        
                    }
                }

                if ((xpath != null) && (xpath.length() != 0)) {
                    targetXPathList.add(xpath);
                }
            } else if (LHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        
                        _xpath = pemImpl.generateXPath(RHTagName);
                        
                        userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//User" + _xpath);
                        resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//AssetResource" + _xpath);
                        
                        for (int i = 0; i < resourceGroupsList.size(); i++) {                                                   
                        	resourceAttr = (Attribute) resourceGroupsList.get(i);
                        	
                            for (int j = 0; j < userGroupsList.size(); j++) {                                                            
                            	userAttr = (Attribute) userGroupsList.get(j);            

                                if (matchOp.equals("=")) {
                                	if (userAttr.getText().equals(resourceAttr.getText())) {
                                		operatorFlag = true;
                                        break;
                                    }
                                } else if (matchOp.equals("!=")) {
                                	if (!userAttr.getText().equals(resourceAttr.getText())) {
                                		operatorFlag = true;
                                        break;
                                    }
                                }
                            }

                            if (operatorFlag) {
                                break;
                            }
                        }

                        if (operatorFlag) {
                            break;
                        }
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();                        
                         
                        _xpath = pemImpl.generateXPath(RHTagName);
                        
    					resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//AssetResource" + _xpath);
    					for (int i = 0; i < resourceGroupsList.size(); i++) {
    						attr = (Attribute) resourceGroupsList.get(i);                                                       
                            operatorFlag = true;                                       
                            xpath.append(" ( (" + RHTagName + matchOp + "'" + attr.getText() + "') )");                                                             
                            if (i < (resourceGroupsList.size() - 1)) {
                            	xpath.append(" or ");
                            }                              
    					}                                                                               
                    }
                }

                if ((xpath != null) && (xpath.length() != 0)) {
                    targetXPathList.add(xpath);
                }
            } else if (LHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_USER)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                        
                        _xpath = pemImpl.generateXPath(RHTagName);
                        
    					userGroupsList = MRXMLUtils.getListXpath(operatorEL, "//User" + _xpath);
    					for (int i = 0; i < userGroupsList.size(); i++) {
    						attr = (Attribute) userGroupsList.get(i);                                                       
                            operatorFlag = true;                                       
                            xpath.append(" ( (" + RHTagName + matchOp + "'" + attr.getText() + "') )");                                                             
                            if (i < (userGroupsList.size() - 1)) {
                            	xpath.append(" or ");
                            }                              
    					}                                      
                    }

                    if ((xpath != null) && (xpath.length() != 0)) {
                        targetXPathList.add(xpath);
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_OPER_RESOURCE)) {
                    xpath = new StringBuffer();

                    for (Iterator<?> objoperatorIT = operatorList.iterator(); objoperatorIT.hasNext();) {
                        operatorEL = (Element) objoperatorIT.next();
                                                
                        _xpath = pemImpl.generateXPath(RHTagName);
                        
    					resourceGroupsList = MRXMLUtils.getListXpath(operatorEL, "//AssetResource" + _xpath);
    					for (int i = 0; i < resourceGroupsList.size(); i++) {
    						attr = (Attribute) resourceGroupsList.get(i);                                                       
                            operatorFlag = true;                                       
                            xpath.append(" ( (" + RHTagName + matchOp + "'" + attr.getText() + "') )");                                                             
                            if (i < (resourceGroupsList.size() - 1)) {
                            	xpath.append(" or ");
                            }                              
    					}                                                                               
                    }

                    if ((xpath != null) && (xpath.length() != 0)) {
                        targetXPathList.add(xpath);
                    }
                } else if (RHRuleKey.equals(PolicyAdminConstants.SERVICE_RULE_TARGET_OBJECT)) {
                }
            }
        }

        if (operatorFlag) {
            countCheck++;
        }
    }

    return countCheck;
}

    private String generateXPath(String tagName)
    {
        int ampIndex = tagName.indexOf("@");
        int spaceIndex = tagName.indexOf(" ");
		String _xpath = tagName.substring(0, ampIndex - 1) + "[" + tagName.substring(ampIndex, spaceIndex) + "]" + tagName.substring(tagName.indexOf("@", spaceIndex) - 1);
		//System.out.println(_xpath);
    	return _xpath;
    }
    
    //Evaluate the rule keys and returns the final xpath
    public static String getAssetAdminPolicyXpath(List<Element> matchingPolicyList, List<?> operatorList, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        Element policyEL = null;

        List<StringBuffer> targetXPathList = new ArrayList<StringBuffer>();
        StringBuffer objectXpath = new StringBuffer("");
        List<?> ruleKeyFilteList = null;
        int countCheck = 0;
        XPathBean objXPathBean = null;
        List<XPathBean> xPathBeanList = new ArrayList<XPathBean>();

        Element filterListEL = getMatchingNodes(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, null, serviceName, request, schema, log);

        List<?> filterList = MRXMLUtils.getList(filterListEL, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE);

        for (Iterator<Element> objIT = matchingPolicyList.iterator(); objIT.hasNext();) {
            countCheck = 0;
            policyEL = objIT.next();

            String allow = policyEL.elementText("allow");

            //boolean isAllow = Boolean.parseBoolean(allow);
            Element objEL = MRXMLUtils.getElement(policyEL, "RuleKeyTagFilterSet");
            ruleKeyFilteList = MRXMLUtils.getList(objEL, "RuleKeyTagFilter");

            if ((ruleKeyFilteList != null) && (ruleKeyFilteList.size() != 0)) {
                countCheck = evaluateRuleKey(ruleKeyFilteList, targetXPathList, operatorList);
            }

            if ((ruleKeyFilteList != null) && (countCheck == ruleKeyFilteList.size())) {
                Element objAssetObjectEL = MRXMLUtils.getElement(policyEL, PolicyAdminConstants.ASSET_ADMIN_OBJECT_FILTER_SET);
                List<?> objectFilterList = MRXMLUtils.getList(objAssetObjectEL, PolicyAdminConstants.ASSET_ADMIN_OBJECT_FILTER_NID);

                //TBD : if object filter list is null and the policy says allow then append *(allow all) to the tag name and value pair
                if ((objectFilterList != null) && (objectFilterList.size() != 0)) {
                    String xpath = getObjectFilterxPath(filterList, objectFilterList);

                    if ((targetXPathList == null) || (targetXPathList.size() == 0)) {
                        if ((xpath != null) && xpath.equals("")) {
                            return "";
                        }
                    }

                    if ((xpath != null) && !xpath.equals("")) {
                        objectXpath = new StringBuffer(xpath);
                    }
                }

                //String strAllow = MRXMLUtils.getValue(policyEL, "allow", "");
                objXPathBean = new XPathBean();
                objXPathBean.setAllow(Boolean.valueOf(allow));
                objXPathBean.setTargetXPath(targetXPathList);
                objXPathBean.setObjectXPath(objectXpath);
                xPathBeanList.add(objXPathBean);
            }
        }

        return generateXPath(xPathBeanList);
    }

    //Evaluate the rule keys and returns the final xpath
    public static String getUserAdminPolicyXpath(List<Element> matchingPolicyList, List<?> operatorList, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        Element policyEL = null;

        List<StringBuffer> targetXPathList = new ArrayList<StringBuffer>();
        StringBuffer objectXpath = new StringBuffer("");
        List<?> ruleKeyFilteList = null;
        int countCheck = 0;
        XPathBean objXPathBean = null;
        List<XPathBean> xPathBeanList = new ArrayList<XPathBean>();

        Element filterListEL = getMatchingNodes(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, null, serviceName, request, schema, log);

        List<?> filterList = MRXMLUtils.getList(filterListEL, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE);

        for (Iterator<Element> objIT = matchingPolicyList.iterator(); objIT.hasNext();) {
            countCheck = 0;
            policyEL = objIT.next();

            Element objEL = MRXMLUtils.getElement(policyEL, "RuleKeyTagFilterSet");
            ruleKeyFilteList = MRXMLUtils.getList(objEL, "RuleKeyTagFilter");

            if ((ruleKeyFilteList != null) && (ruleKeyFilteList.size() != 0)) {
                countCheck = evaluateRuleKey(ruleKeyFilteList, targetXPathList, operatorList);
            }

            if ((ruleKeyFilteList != null) && (countCheck == ruleKeyFilteList.size())) {
                Element objAssetObjectEL = MRXMLUtils.getElement(policyEL, PolicyAdminConstants.USER_ADMIN_OBJECT_FILTER_SET);
                List<?> objectFilterList = MRXMLUtils.getList(objAssetObjectEL, PolicyAdminConstants.USER_ADMIN_OBJECT_FILTER_NID);

                if ((objectFilterList != null) && (objectFilterList.size() != 0)) {
                    String xpath = getObjectFilterxPath(filterList, objectFilterList);

                    if ((xpath != null) && xpath.equals("")) {
                        if ((targetXPathList == null) || (targetXPathList.size() == 0)) {
                            return "";
                        }
                    }

                    if ((xpath != null) && !xpath.equals("")) {
                        objectXpath = new StringBuffer(xpath);
                    }

                    //System.out.println(objectXpath);
                }

                String strAllow = MRXMLUtils.getValue(policyEL, "allow", "");
                objXPathBean = new XPathBean();
                objXPathBean.setAllow(Boolean.valueOf(strAllow));
                objXPathBean.setTargetXPath(targetXPathList);
                objXPathBean.setObjectXPath(objectXpath);
                xPathBeanList.add(objXPathBean);
            }
        }

        //System.out.println("ObjectXPATH 23456 ----------->" + generateXPath(xPathBeanList));
        return generateXPath(xPathBeanList);
    }

    private static String getObjectFilterxPath(List<?> filterList, List<?> objectFilterList)
        throws MRException, Exception {
        Element objectEL = null;
        Element filterEL = null;
        String objectNID = null;
        String filterNID = null;
        StringBuffer objectXpath = null;
       
        for (int i = 0; i < objectFilterList.size(); i++) {
            objectEL = (Element) objectFilterList.get(i);
            objectNID = MRXMLUtils.getValue(objectEL);
            String filterXpath =  null;
            if ((filterList != null) && (filterList.size() != 0)) {
                for (int j = 0; j < filterList.size(); j++) {
                    filterEL = (Element) filterList.get(j);
                    filterNID = MRXMLUtils.getAttributeValue(filterEL, PolicyAdminConstants.NID, "");

                    if (filterNID.equals(objectNID)) {
                        String name = filterEL.elementText("Name");

                        if (name.equals(PolicyAdminConstants.ALL_FILTER)) {
                            return "";
                        }
                        filterXpath = MediaRoomPolicyHelper.generateFilterXpath(filterEL);                 
                        break;
                    }
                }
                if(filterXpath != null && !filterXpath.isEmpty() && objectXpath == null) {
                	objectXpath = new StringBuffer();
                }
                
                if(filterXpath!= null && !filterXpath.isEmpty()){
                	objectXpath.append(filterXpath);  
                	if (i < ((objectFilterList.size()) - 1)) {
                        objectXpath.append(" or ");
                    }
                }                
            }
        }

        if (objectXpath != null && !objectXpath.toString().isEmpty()) {           
            return objectXpath.toString();
        }

        return null;
    }

    //generate the xpath
    private static String generateXPath(List<XPathBean> xPathBeanList) {
        StringBuffer pemXPath = null;
        //StringBuffer xPath = new StringBuffer();
        StringBuffer xPath = null;
        StringBuffer subXPath = new StringBuffer();

        XPathBean objBean = null;
        XPathBean nextBean = null;
        List<StringBuffer> targetXPathList = null;
        boolean isFirstDeny = true;
        boolean initial = true;

        if ((xPathBeanList != null) && (xPathBeanList.size() != 0)) {
            // pemXPath.append("//" + PolicyAdminConstants.TAG + "[");
            for (int i = 0; i < xPathBeanList.size(); i++) {
                xPath = new StringBuffer();
                objBean = xPathBeanList.get(i);

                if (i < (xPathBeanList.size() - 1)) {
                    nextBean = xPathBeanList.get(i + 1);
                }

                targetXPathList = objBean.getTargetXPath();

                if (((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) || ((targetXPathList != null) && (targetXPathList.size() != 0))) {
                    if (initial) {
                        pemXPath = new StringBuffer();
                        //pemXPath.append("//" + PolicyAdminConstants.TAG + "[");
                        pemXPath.append("[");
                        initial = false;
                    }

                    if (objBean.isAllow()) {
                        xPath.append("( ");
                    } else {
                        xPath.append(" ( not ( ");
                    }

                    if ((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) {
                        xPath.append(" ( ");
                        xPath.append(objBean.getObjectXPath());

                        xPath.append(" ) ");
                    }

                    //targetXPathList = objBean.getTargetXPath();
                    if ((targetXPathList != null) && (targetXPathList.size() != 0)) {
                        if ((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) {
                            xPath.append(" and  ");
                        }

                        xPath.append(" ( ");

                        for (Iterator<StringBuffer> objIT = targetXPathList.iterator(); objIT.hasNext();) {
                            String targetXPath = "" + objIT.next();

                            if ((targetXPath != null) && (targetXPath.trim().length() != 0)) {
                                xPath.append(" ( ");

                                xPath.append(targetXPath);
                                xPath.append(" ) ");

                                if (objIT.hasNext()) {
                                    xPath.append(" and ");
                                }
                            }
                        }

                        xPath.append(" ) ");
                    }

                    if (objBean.isAllow()) {
                        xPath.append(" ) ");
                    } else {
                        xPath.append(" ) ) ");
                    }

                    subXPath.append(xPath.toString());

                    if (i < (xPathBeanList.size() - 1)) {
                        if (objBean.isAllow() && nextBean.isAllow()) {
                            if (i < (xPathBeanList.size() - 1)) {
                                subXPath.append(" or ");
                            }
                        } else if (objBean.isAllow() && !nextBean.isAllow()) {
                            if (isFirstDeny) {
                                isFirstDeny = false;

                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" or ");
                                }

                                pemXPath.append(subXPath);

                                subXPath = new StringBuffer();
                                subXPath.append(" ( ( ");
                            } else {
                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" ) and ( ( ");
                                }
                            }
                        } else if (!objBean.isAllow() && nextBean.isAllow()) {
                            if (isFirstDeny) {
                                isFirstDeny = false;

                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" and ");
                                }

                                pemXPath.append(" ( ( ");
                                pemXPath.append(subXPath);
                                subXPath = new StringBuffer();
                            } else {
                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(") and ( ");
                                }
                            }
                        } else if (!objBean.isAllow() && !nextBean.isAllow()) {
                            if (isFirstDeny) {
                                isFirstDeny = false;

                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" or ");
                                }

                                pemXPath.append("( ( ");
                                pemXPath.append(subXPath);

                                subXPath = new StringBuffer();
                            } else {
                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" or ");
                                }
                            }
                        }
                    }
                }
            }

            if (!isFirstDeny) {
                subXPath.append(" ) ) ");
            }

            if (pemXPath != null) {
                pemXPath.append(subXPath);
            }

            if (!initial) {
                if (pemXPath != null) {
                    pemXPath.append(" ] ");
                }
            }
        }

        if (pemXPath != null) {
            //System.out.println("ObjectXPATH----------->" + pemXPath.toString());
            return pemXPath.toString();
        }

        return null;
    }

    private static String generatePolicyAdminXPath(List<XPathBean> xPathBeanList) {
        StringBuffer pemXPath = new StringBuffer();
        //StringBuffer xPath = new StringBuffer();
        StringBuffer xPath = null;
        StringBuffer subXPath = new StringBuffer();

        XPathBean objBean = null;
        XPathBean nextBean = null;
        List<StringBuffer> targetXPathList = null;
        boolean isFirstDeny = true;

        if ((xPathBeanList != null) && (xPathBeanList.size() != 0)) {
            pemXPath.append(" [");

            for (int i = 0; i < xPathBeanList.size(); i++) {
                xPath = new StringBuffer();
                objBean = xPathBeanList.get(i);

                if (i < (xPathBeanList.size() - 1)) {
                    nextBean = xPathBeanList.get(i + 1);
                }

                if (((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) || ((targetXPathList != null) && (targetXPathList.size() != 0))) {
                    if (objBean.isAllow()) {
                        xPath.append("( ");
                    } else {
                        xPath.append(" ( not ( ");
                    }
                }

                if ((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) {
                    xPath.append(" ( ");
                    xPath.append(objBean.getObjectXPath());
                    xPath.append(" ) ");
                }

                targetXPathList = objBean.getTargetXPath();

                if ((targetXPathList != null) && (targetXPathList.size() != 0)) {
                    if ((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().length() != 0)) {
                        xPath.append(" and  ");
                    }

                    xPath.append(" ( ");

                    for (Iterator<StringBuffer> objIT = targetXPathList.iterator(); objIT.hasNext();) {
                        String targetXPath = "" + objIT.next();
                        xPath.append(" ( ");

                        xPath.append(targetXPath);
                        xPath.append(" ) ");

                        if (objIT.hasNext()) {
                            xPath.append(" and ");
                        }
                    }

                    xPath.append(" ) ");
                }

                if (((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) || ((targetXPathList != null) && (targetXPathList.size() != 0))) {
                    if (objBean.isAllow()) {
                        xPath.append(" ) ");
                    } else {
                        xPath.append(" ) ) ");
                    }
                }

                subXPath.append(xPath.toString());

                if (((objBean.getObjectXPath() != null) && (objBean.getObjectXPath().toString().trim().length() != 0)) || ((targetXPathList != null) && (targetXPathList.size() != 0))) {
                    if (i < (xPathBeanList.size() - 1)) {
                        if (objBean.isAllow() && nextBean.isAllow()) {
                            if (i < (xPathBeanList.size() - 1)) {
                                subXPath.append(" or ");
                            }
                        } else if (objBean.isAllow() && !nextBean.isAllow()) {
                            if (isFirstDeny) {
                                isFirstDeny = false;

                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" or ");
                                }

                                pemXPath.append(subXPath);

                                subXPath = new StringBuffer();
                                subXPath.append(" ( ( ");
                            } else {
                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" ) and ( ( ");
                                }
                            }
                        } else if (!objBean.isAllow() && nextBean.isAllow()) {
                            if (isFirstDeny) {
                                isFirstDeny = false;

                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" and ");
                                }

                                pemXPath.append(" ( ( ");
                                pemXPath.append(subXPath);
                                subXPath = new StringBuffer();
                            } else {
                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(") and ( ");
                                }
                            }
                        } else if (!objBean.isAllow() && !nextBean.isAllow()) {
                            if (isFirstDeny) {
                                isFirstDeny = false;

                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" or ");
                                }

                                pemXPath.append("( ( ");
                                pemXPath.append(subXPath);

                                subXPath = new StringBuffer();
                            } else {
                                if (i < (xPathBeanList.size() - 1)) {
                                    subXPath.append(" or ");
                                }
                            }
                        }
                    }
                }
            }

            if (!isFirstDeny) {
                subXPath.append(" ) ) ");
            }

            if (subXPath.toString().trim().length() != 0) {
                pemXPath.append(subXPath);
                pemXPath.append(" ] ");
            } else {
                pemXPath = null;
            }
        }

        if (pemXPath != null) {
            return pemXPath.toString();
        }

        return null;
    }

    //sort the polices based on order
    public static List<Element> sortPolicesBasedOnOrder(List<?> matchingPolicyList)
        throws MRException, Exception {
        Element objEL = null;
        Element objInnerEL = null;
        String order = null;
        String innerLoopOrder = null;
        List<Element> policyList = new ArrayList<Element>();
        Object[] objListArray = matchingPolicyList.toArray();

        for (int i = 0; i < (objListArray.length - 1); i++) {
            for (int j = i + 1; j < (matchingPolicyList.size()); j++) {
                objEL = (Element) objListArray[i];
                order = MRXMLUtils.getValue(objEL, "order", "");

                objInnerEL = (Element) objListArray[j];
                innerLoopOrder = MRXMLUtils.getValue(objInnerEL, "order", "");

                if ((Integer.parseInt(innerLoopOrder.trim())) < (Integer.parseInt(order.trim()))) {
                    objListArray[j] = objListArray[i];
                    objListArray[i] = objInnerEL;
                }
            }
        }

        for (int i = 0; i < objListArray.length; i++) {
            objEL = (Element) objListArray[i];
            policyList.add(objEL);
        }

        return policyList;
    }

    public static String getPolicyAdminXpath(List<Element> matchingPolicyList, List<?> operatorList, String objectType, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        Element policyEL = null;

        List<StringBuffer> ruleKeyXPathList = new ArrayList<StringBuffer>();
        StringBuffer objectXpath = new StringBuffer("");
        XPathBean objXPathBean = null;
        List<XPathBean> xPathBeanList = new ArrayList<XPathBean>();

        Element filterListEL = getMatchingNodes(PolicyAdminConstants.OBJECT_FILTER_OBJECTYPE, null, serviceName, request, schema, log);

        List<?> filterList = MRXMLUtils.getList(filterListEL, PolicyAdminConstants.OBJECT_FILTER_OBJECTYPE);
        List<String> objectXpathList = null;

        for (Iterator<Element> objIT = matchingPolicyList.iterator(); objIT.hasNext();) {
            policyEL = objIT.next();

            Element objPolicyObjectEL = MRXMLUtils.getElement(policyEL, PolicyAdminConstants.POLICY_ADMIN_OBJECT_FILTER_SET);

            List<?> objectFilterList = MRXMLUtils.getList(objPolicyObjectEL, PolicyAdminConstants.POLICY_ADMIN_OBJECT_FILTER_NID);

            if ((objectFilterList != null) && (objectFilterList.size() != 0)) {
                Element objectEL = null;
                Element filterEL = null;
                String objectNID = null;
                String filterNID = null;
                objectXpath = null;
                objectXpathList = new ArrayList<String>();

                //get the tag name and value pair for target objects
                for (int i = 0; i < objectFilterList.size(); i++) {
                    objectEL = (Element) objectFilterList.get(i);
                    objectNID = MRXMLUtils.getValue(objectEL);

                    if ((filterList != null) && (filterList.size() != 0)) {
                        if ((objectNID != null) && (objectNID.trim().length() != 0)) {
                            for (int j = 0; j < filterList.size(); j++) {
                                filterEL = (Element) filterList.get(j);
                                filterNID = MRXMLUtils.getAttributeValue(filterEL, PolicyAdminConstants.NID, "");

                                if (filterNID.equals(objectNID)) {
                                    Element tagFilterListEL = MRXMLUtils.getElement(filterEL, PolicyAdminConstants.ATTRIBUTE_FILTER_SET);
                                    List<?> tagFilterList = MRXMLUtils.getList(tagFilterListEL, PolicyAdminConstants.ATTRIBUTE_FILTER);

                                    if ((tagFilterList != null) && (tagFilterList.size() != 0)) {
                                        for (Iterator<?> objfilterIT = tagFilterList.iterator(); objfilterIT.hasNext();) {
                                            Element tagFilterEL = (Element) objfilterIT.next();
                                            String noTagObjectType = MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.NO_TAG_OBJECT_TYPE, "").trim();
                                            objectXpath = new StringBuffer();

                                            if (!objectType.equals(noTagObjectType)) {
                                                continue;
                                            }

                                            String tagXpath = MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.LH_OBJECT_RELATIVE_XPATH, "").trim();

                                            if ((tagXpath != null) && (tagXpath.length() != 0)) {
                                                /*  if (inital) {
                                                      objectXpath = new StringBuffer();
                                                      objectXpath.append(" ( ");
                                                      inital = false;
                                                  }*/
                                                objectXpath.append(" ( ");

                                                String regexp = MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.FILTER_TAG_RHREGEXP, "").trim();

                                                if (regexp.equals("*")) {
                                                    objectXpath.append(MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.LH_OBJECT_RELATIVE_XPATH, ""));
                                                } else {
                                                	String matchOp = MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.MATCH_OPERATOR, "");
                                                	matchOp = MediaRoomPolicyHelper.getMatchOperator(matchOp);
                                                        objectXpath.append(MRXMLUtils.buildXPath(MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.LH_OBJECT_RELATIVE_XPATH, "").trim(),
                                                                MRXMLUtils.getValue(tagFilterEL, PolicyAdminConstants.FILTER_TAG_RHREGEXP, "").trim(),matchOp));                                                  
                                                }

                                                objectXpath.append(" ) ");
                                                objectXpathList.add(objectXpath.toString());

                                                /*if (objfilterIT.hasNext()) {
                                                    objectXpath.append(" or ");
                                                }*/
                                            }
                                        }
                                    }

                                    break;
                                }
                            }

                            /*if (!inital) {
                                objectXpath.append(" ) ");
                            
                                if (i < ((objectFilterList.size()) - 1)) {
                                    objectXpath.append(" or ");
                                    objectXpath.append(" ( ");
                                }
                            }*/
                        }
                    }
                }

                objectXpath = generateObjectXpathFromList(objectXpathList);

                String strAllow = MRXMLUtils.getValue(policyEL, "allow", "");
                objXPathBean = new XPathBean();
                objXPathBean.setAllow(Boolean.valueOf(strAllow));
                objXPathBean.setTargetXPath(ruleKeyXPathList);

                if ((objectXpath != null) && (objectXpath.toString().length() != 0)) {
                    objXPathBean.setObjectXPath(objectXpath);
                }

                xPathBeanList.add(objXPathBean);
            }
        }

        return generatePolicyAdminXPath(xPathBeanList);
    }

    private static StringBuffer generateObjectXpathFromList(List<String> objectXpathList) {
        Iterator<String> objectxpathListIterator = objectXpathList.iterator();
        StringBuffer xPath = new StringBuffer();
        int count = 0;
        int maxCount = objectXpathList.size();

        while (objectxpathListIterator.hasNext()) {
            xPath.append(objectxpathListIterator.next());

            if (count != (maxCount - 1)) {
                xPath.append(" or ");
            }

            count++;
        }

        return xPath;
    }

    public static String getMediaStoreAdminXPath(List<Element> matchingPolicyList, List<?> operatorList, String serviceName, MRRequest request, MRDB schema, MRLog log)
        throws MRException, Exception {
        Element policyEL = null;

        List<StringBuffer> targetXPathList = new ArrayList<StringBuffer>();
        StringBuffer objectXpath = new StringBuffer("");
        List<?> ruleKeyFilteList = null;
        int countCheck = 0;
        XPathBean objXPathBean = null;
        List<XPathBean> xPathBeanList = new ArrayList<XPathBean>();

        Element filterListEL = getMatchingNodes(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, null, serviceName, request, schema, log);

        List<?> filterList = MRXMLUtils.getList(filterListEL, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE);

        for (Iterator<Element> objIT = matchingPolicyList.iterator(); objIT.hasNext();) {
            countCheck = 0;
            policyEL = objIT.next();

            Element objEL = MRXMLUtils.getElement(policyEL, "RuleKeyTagFilterSet");
            ruleKeyFilteList = MRXMLUtils.getList(objEL, "RuleKeyTagFilter");

            if ((ruleKeyFilteList != null) && (ruleKeyFilteList.size() != 0)) {
                countCheck = evaluateRuleKey(ruleKeyFilteList, targetXPathList, operatorList);
            }

            if ((ruleKeyFilteList != null) && (countCheck == ruleKeyFilteList.size())) {
                Element objAssetObjectEL = MRXMLUtils.getElement(policyEL, PolicyAdminConstants.MEDIA_STORE_OBJECT_FILTER_SET);
                List<?> objectFilterList = MRXMLUtils.getList(objAssetObjectEL, PolicyAdminConstants.MEDIA_STORE_OBJECT_FILTER_NID);

                if ((objectFilterList != null) && (objectFilterList.size() != 0)) {
                    String xpath = getObjectFilterxPath(filterList, objectFilterList);

                    if ((xpath != null) && xpath.equals("")) {
                        if ((targetXPathList == null) || (targetXPathList.size() == 0)) {
                            return "";
                        }
                    }

                    if ((xpath != null) && !xpath.equals("")) {
                        objectXpath = new StringBuffer(xpath);
                    }
                }

                String strAllow = MRXMLUtils.getValue(policyEL, "allow", "");
                objXPathBean = new XPathBean();
                objXPathBean.setAllow(Boolean.valueOf(strAllow));
                objXPathBean.setTargetXPath(targetXPathList);
                objXPathBean.setObjectXPath(objectXpath);
                xPathBeanList.add(objXPathBean);
            }
        }

        //System.out.println("ObjectXPATH 23456 ----------->" + generateXPath(xPathBeanList));
        return generateXPath(xPathBeanList);
    }
}
