package com.ipvs.policyadminservice.impl;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;


/**
 * @author padmaja
 *
 */
public class PolicyManagementValidation {
    public static final String NID = "NID";

    @SuppressWarnings("unchecked")
    public static void validateStreamPolicy(Element actionEL, String serviceName, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        String nidValue = null;

        nidValue = MRXMLUtils.getValue(actionEL, NID, null);

        //Element parentEl = MRXMLUtils.newElement("ObjectList");
        MRConfig config = null;
        config = MRConfig.getInstance(serviceName, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, log);

        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element whereAssistXML = MRXMLUtils.newElement("WhereAssistXML");
        Element whereXPathEL = MRXMLUtils.newElement("WhereXPath");
        whereXPathEL.setText("//" + PolicyAdminConstants.MEDIA_ROOM_STREAM_FILTER_SET + "[" +
            MRXMLUtils.buildXPath(PolicyAdminConstants.MEDIA_STREAM_FILTER_NID, nidValue) + "]");

        String whereXPath = MRXMLUtils.getValue(whereXPathEL);

        Element childEL = MRXMLUtils.newElement(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE + "List");
        int whereSelectNodeDepth = 1;
        Element ObjectListEL = validate(parentNID, childEL, null, whereSelectNodeDepth, whereAssistXML, whereXPath, request,
                db, log);

        // Document objectListdoc = MRXMLUtils.getDocument();
        //System.out.println(MRXMLUtils.elementToString(ObjectListEL));
        if ((ObjectListEL != null)) {
            //MRXMLUtils.addElement(parentEl, ObjectListEL);
            List<Element> policyList = ObjectListEL.elements();

            if ((policyList != null) && (policyList.size() != 0)) {
                Element objEL = null;
                List<?> streamFilterNIDList = null;
                String policyNID = null;

                for (Iterator<Element> objIT = policyList.iterator(); objIT.hasNext();) {
                    objEL = objIT.next();
                    policyNID = MRXMLUtils.getAttributeValue(objEL,"NID");
                    streamFilterNIDList = objEL.element("MediaRoomStreamFilterSpec")
                                               .element(PolicyAdminConstants.MEDIA_ROOM_STREAM_FILTER_SET).elements();

                    if ((streamFilterNIDList != null) && (streamFilterNIDList.size() != 0)) {
                        String streamFilternidValue = null;
                        Element nidEL = null;

                        for (Iterator<?> nidIT = streamFilterNIDList.iterator(); nidIT.hasNext();) {
                            nidEL = (Element)nidIT.next();
                            streamFilternidValue = MRXMLUtils.getValue(nidEL);

                            if ((streamFilternidValue != null) && (streamFilternidValue.trim().length() != 0)) {
                                if (streamFilternidValue.equals(nidValue)) {
                                    nidEL.setText("");
                                    update(policyNID, objEL, request, db, log);

                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // MRXMLUtils.addElement(objectListdoc, parentEl);
        }
    }

    private static void update(String parentNID, Element objectEL, MRRequest request, MRDB db, MRLog log)
        throws Exception {
        MRXMLDBUtils.setObjectData(request, db, log, parentNID, MRXMLUtils.elementToString(objectEL));
    }

    private static Element validate(String parentNID, Element responceEL, Element operatonData, int whereSelectNodeDepth,
        Element whereAssistXML, String whereXPath, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        String[] arrXpath = { whereXPath };
        MRXMLDBUtils.findChildElements(parentNID, responceEL, operatonData, whereSelectNodeDepth, whereAssistXML, arrXpath,
            request, db, log);

        if ((responceEL != null) && (responceEL.elements().size() != 0)) {
            return responceEL;
        }

        return null;
    }

    public static void validateFilter(Element actionEL, String serviceName, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        String nidValue = null;
        nidValue = MRXMLUtils.getValue(actionEL, NID, null);

        Element objectListEL = null;

        String xpath = PolicyAdminConstants.USER_FILTER_NID + "=" + nidValue;

        String sourceFilterXPath = "(" + PolicyAdminConstants.SRC_FILTER_SET + "/" + PolicyAdminConstants.SRC_FILTER + "/" +
            PolicyAdminConstants.USER_FILTER_NID + " = '" + nidValue + "') or (" + PolicyAdminConstants.SRC_FILTER_SET + "/" +
            PolicyAdminConstants.SRC_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID + " = '" + nidValue + "')";

        String profileFilterXPath = "(" + PolicyAdminConstants.PROFILE_FILTER_SET + "/" + PolicyAdminConstants.PROFILE_FILTER +
            "/" + PolicyAdminConstants.STREAM_PROFILE_FILTER_NID + " = '" + nidValue + "') ";

        String destFilterXPath = "(" + PolicyAdminConstants.DST_FILTER_SET + "/" + PolicyAdminConstants.DST_FILTER + "/" +
            PolicyAdminConstants.USER_FILTER_NID + " = '" + nidValue + "') or (" + PolicyAdminConstants.DST_FILTER_SET + "/" +
            PolicyAdminConstants.DST_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID + " = '" + nidValue + "')";

        String relayFilterXPath = "(" + PolicyAdminConstants.RELAY_FILTER_SET + "/" + PolicyAdminConstants.RELAY_FILTER + "/" +
            PolicyAdminConstants.USER_FILTER_NID + " = '" + nidValue + "') or (" + PolicyAdminConstants.RELAY_FILTER_SET + "/" +
            PolicyAdminConstants.RELAY_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID + " = '" + nidValue + "')";

        String pvrFilterXPath = "(" + PolicyAdminConstants.PVR_FILTER_SET + "/" + PolicyAdminConstants.PVR_FILTER + "/" +
            PolicyAdminConstants.USER_FILTER_NID + " = '" + nidValue + "') or (" + PolicyAdminConstants.PVR_FILTER_SET + "/" +
            PolicyAdminConstants.PVR_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID + " = '" + nidValue + "')";

        String serviceFilterXPath = "(" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER_SET + "/" +
            PolicyAdminConstants.SERVICE_OPERATOR_FILTER + "/" + PolicyAdminConstants.USER_FILTER_NID + " = '" + nidValue +
            "') or (" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER_SET + "/" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER +
            "/" + PolicyAdminConstants.RESOURCE_FILTER_NID + " = '" + nidValue + "')";

        //stream policy cheking
        xpath = "//" + PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE + "[" + sourceFilterXPath + " or " + destFilterXPath +
            " or " + profileFilterXPath + " or " + relayFilterXPath + " or " + pvrFilterXPath + "]";

        objectListEL = setValues(PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            updatePolices(nidValue, objectListEL, PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, request, db, log);
        }

        // Room Policy Checking
        xpath = "//" + PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE + " [ " + serviceFilterXPath + " or " +
            sourceFilterXPath + " or " + destFilterXPath + " or (" + PolicyAdminConstants.MEDIA_ROOM_OBJECT_FILTER_SET + "/" +
            PolicyAdminConstants.MEDIA_ROOM_OBJECT_FILTER_NID + " = '" + nidValue + "')]";

        objectListEL = setValues(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            updatePolices(nidValue, objectListEL, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, request, db, log);
        }

        // Asset Admin Policy Checking
        xpath = "//" + PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE + " [ " + serviceFilterXPath + " or (" +
            PolicyAdminConstants.ASSET_ADMIN_OBJECT_FILTER_SET + "/" + PolicyAdminConstants.ASSET_ADMIN_OBJECT_FILTER_NID +
            " = '" + nidValue + "')]";

        objectListEL = setValues(PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            updatePolices(nidValue, objectListEL, PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE, request, db, log);
        }

        // Policy Admin Policy Checking
        xpath = "//" + PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE + " [ " + serviceFilterXPath + " ]";

        objectListEL = setValues(PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            updatePolices(nidValue, objectListEL, PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE, request, db, log);
        }

        //Media Store Admin Policy Checking
        xpath = "//" + PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE + " [ " + serviceFilterXPath + " or (" +
            PolicyAdminConstants.MEDIA_STORE_OBJECT_FILTER_SET + "/" + PolicyAdminConstants.MEDIA_STORE_OBJECT_FILTER_NID +
            " = '" + nidValue + "')]";

        objectListEL = setValues(PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db,
                log);

        if (objectListEL != null) {
            updatePolices(nidValue, objectListEL, PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE, request, db, log);
        }

        // User Admin Policy Rule Checking
        xpath = "//" + PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE + " [ " + serviceFilterXPath + " or (" +
            PolicyAdminConstants.USER_ADMIN_OBJECT_FILTER + "/" + PolicyAdminConstants.USER_ADMIN_OBJECT_FILTER_NID + " = '" +
            nidValue + "')]";

        objectListEL = setValues(PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            updatePolices(nidValue, objectListEL, PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE, request, db, log);
        }
    }

    @SuppressWarnings("unchecked")
    private static void updatePolices(String nid, Element objectListEL, String objectType, MRRequest request, MRDB db,
        MRLog log) throws Exception {
        List<Element> objectList = objectListEL.elements(objectType);      

        if ((objectList != null) && (objectList.size() != 0)) {
            Element objEL = null;
            String objectXML = null;
            String policyNId = null;
            for (Iterator<Element> objIT = objectList.iterator(); objIT.hasNext();) {
                objEL = objIT.next();
                policyNId = MRXMLUtils.getAttributeValue(objEL,PolicyAdminConstants.NID);
                objectXML = MRXMLUtils.elementToString(objEL);               
                objectXML = objectXML.replace(nid, "");       
                MRXMLDBUtils.setObjectData(request, db, log, policyNId, objectXML);
            }
        }
    }

    private static Element setValues(String objectType, String xpath, String serviceName, MRRequest request, MRDB db,
        MRLog log) throws MRException, Exception {
        MRConfig config = null;
        config = MRConfig.getInstance(serviceName, objectType, log);

        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element whereAssistXML = MRXMLUtils.newElement("WhereAssistXML");
        Element whereXPathEL = MRXMLUtils.newElement("WhereXPath");
        whereXPathEL.setText(xpath);

        String whereXPath = MRXMLUtils.getValue(whereXPathEL);
        Element childEL = MRXMLUtils.newElement(objectType + "List");

        int whereSelectNodeDepth = 1;

        Element objectList = validate(parentNID, childEL, null, whereSelectNodeDepth, whereAssistXML, whereXPath, request,
                db, log);

        return objectList;
    }

    public static Document validateGroupTagDef(Element actionEL, String TagName, String serviceName, MRRequest request,
        MRDB db, MRLog log) throws MRException, Exception {
        Document objectListDoc = MRXMLUtils.getDocument();
        Element parentEl = MRXMLUtils.newElement("ObjectList");
        Element objectListEL = null;

        //Filter Checking
        String xpath = "//" + PolicyAdminConstants.TAG_FILTER + "[" +
            MRXMLUtils.buildXPath(PolicyAdminConstants.LH_TAG_NAME, TagName) + "]";
        objectListEL = setValues(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, xpath, serviceName, request, db, log);

        //System.out.println("XPATH $$$$$$"+xpath);
        if (objectListEL != null) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        //Media Stream Policy Checking
        xpath = "//" + PolicyAdminConstants.RULE_KEY_TAG_FILTER + " [  (" + PolicyAdminConstants.RULE_KEY_LH_TAG_NAME + "/" +
            PolicyAdminConstants.TAG_NAME + " = '" + TagName + "') or ( " + PolicyAdminConstants.RULE_KEY_RH_TAG_NAME + "/" +
            PolicyAdminConstants.TAG_NAME + " = '" + TagName + "')]";

        // System.out.println("XPATH $$$$$$"+xpath);
        objectListEL = setValues(PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        //Media Room Policy checking        
        objectListEL = setValues(PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        //Asset Admin Policy checking        
        objectListEL = setValues(PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        //Policy Admin Policy checking        
        objectListEL = setValues(PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        //User Admin Policy checking        
        objectListEL = setValues(PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db, log);

        if (objectListEL != null) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        //Media Store Admin Policy checking        
        objectListEL = setValues(PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE, xpath, serviceName, request, db,
                log);

        if ((objectListEL != null)) {
            MRXMLUtils.addElement(parentEl, objectListEL);
        }

        if (parentEl != null) {
            MRXMLUtils.addElement(objectListDoc, parentEl);

            if (parentEl.content().size() != 0) {
                return objectListDoc;
            }
        }

        return null;
    }

    public static boolean validatePEM(Element actionEL, String pemXPath, String objectType, MRRequest request, MRDB db,
        MRLog log) throws Exception {
        String parentNID = MRXMLUtils.getValue(actionEL, "NID", "");       
        Element responseXML = MRXMLUtils.newElement("ResponseXML");
        Element whereAssistXML = MRXMLUtils.newElement("WhereAssistXML");
        String[] whereXPathArray = { pemXPath };
        MRXMLDBUtils.readElement(1, whereAssistXML, whereXPathArray, parentNID, responseXML, request, log, db);

        if (responseXML != null) {
            Element objEL = responseXML.element(objectType);

            if (objEL != null) {
                return true;
            }
        }

        return false;
    }
    
    public static void validateObjectFilter(Element actionEL,
			String serviceName, MRRequest request, MRDB db, MRLog log) throws Exception , MRException {


		 String nidValue = null;
	        nidValue = MRXMLUtils.getValue(actionEL, NID, null);
	        Element objectListEL = null;	  
	        String xpath = PolicyAdminConstants.USER_FILTER_NID + "=" + nidValue;

	        String objectFilterXPath = "(" + PolicyAdminConstants.POLICY_ADMIN_OBJECT_FILTER_SET +
              		"/" + PolicyAdminConstants.POLICY_ADMIN_OBJECT_FILTER_NID + " = '" + nidValue +
              		"') ";
	           // Policy Admin Policy Checking
	        xpath = "//" + PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE +
	            " [ " + objectFilterXPath + "]";
	      
	        objectListEL = setValues(PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE,
	                xpath, serviceName, request, db, log);
	        if (objectListEL != null) {
	        	 updatePolices(nidValue, objectListEL, PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE, request, db, log);
	        }

	 
	}
}
