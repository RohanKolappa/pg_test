package com.ipvs.policyadminservice.utils;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.handler.GetTargetXPathRequestHandler;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.impl.TargetObjectLib;


public class PolicyAdminServiceUtils {
	
    public static Document getTargetXpathResponse(MRRequest requestObject, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,  List<String> roleObjectList,
        String action) throws Exception {
    	String requestName = MRRequest.getRequestName(GetTargetXPathRequestHandler.class.getName());
    	Document policyRequestDoc = PolicyAdminServiceUtils.getPEMRequest(log, requestName);

        //get operator document 
        Document operatorDoc = TargetObjectLib.createRequestDoc(requestObject, xmpp, log, db, resourceFactory, serviceAgentFactory);
        Element operatorEL = MRXMLUtils.getElementXpath(operatorDoc, "//"+PolicyAdminConstants.OPERATOR);      
        MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(policyRequestDoc, "//"+PolicyAdminConstants.OPERATOR), operatorEL);
        Element targetObjectRoleEL = (Element)MRXMLUtils.getElementXpath(policyRequestDoc, "//"+PolicyAdminConstants.TARGET_OBJECT_ROLE).detach();
        Element targetObjectRoleSetEL = MRXMLUtils.getElementXpath(policyRequestDoc, "//"+PolicyAdminConstants.TARGET_OBJECT_ROLE_SET);
        for(String roleObject : roleObjectList) {        	
        	  MRXMLUtils.setValueXpath(targetObjectRoleEL, "./"+PolicyAdminConstants.ROLE_OBJECT,roleObject);
              MRXMLUtils.setValueXpath(targetObjectRoleEL, "./"+PolicyAdminConstants.ROLE_ACTION,action);
              MRXMLUtils.addElement(targetObjectRoleSetEL,(Element)targetObjectRoleEL.clone());            
        }
        MRRequest request =  MRRequest.createRequest(requestObject, policyRequestDoc, requestName, ServiceClient.POLICYADMINSERVICE);
        MRRequestHandlerI handler = new GetTargetXPathRequestHandler();
        Document policyResponseDoc = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory); 
        return policyResponseDoc;
    }

    private static Document getPEMRequest(MRLog log, String requestName) throws MRException, Exception {
        String xmlFilePath = "/com/ipvs/policyadminservice/xml/" + requestName + ".xml";

        return MRXMLUtils.loadXMLResource(xmlFilePath, log);
    }

    public static String getTargetXpath(List<String> roleObjectList, String dbObjectType, String action, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
    	
        if(request.isSuperUser()) {
        	return "";
        }
      
        Document responseDoc =  getTargetXpathResponse(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, roleObjectList, action);
        return validateTargetXPath(responseDoc, dbObjectType, roleObjectList, action);
        
    }
    
    public static String validateTargetXPath(Document responseDoc, String dbObjectType,List<String> roleObject,String action) throws Exception{
    	String targetXPath = MRXMLUtils.getValueXpath(responseDoc, "//"+PolicyAdminConstants.TARGET_XPATH, "null");
    	if(targetXPath.equals("null")){
        	throw new MRException(PolicyAdminServiceUtils.getExceptionCode(action), MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectType));
        }
        
    	 //If the object type is export import list just make sure user has access. 
        // no need to apply for xpath on target xpath because export import does not have any groups 
        if (roleObject.size()==1 && roleObject.contains(AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE)) {
        	return "";
        }
        
        if(targetXPath.length() !=0 ) {
        	return "[" + targetXPath  +  "]";
        }
         
        return targetXPath;
        
    }

    public static int getExceptionCode(String action) {
        if (action.equals(PolicyAdminConstants.ADD_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_ADD;
        } else if (action.equals(PolicyAdminConstants.MODIFY_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_UPDATE;
        } else if (action.equals(PolicyAdminConstants.READ_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_READ;
        } else if (action.equals(PolicyAdminConstants.DELETE_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_DELETE;
        } else if (action.equals(PolicyAdminConstants.IMPORT_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_IMPORT;
        } else if (action.equals(PolicyAdminConstants.EXPORT_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_EXPORT;
        } else if (action.equals(PolicyAdminConstants.UPGRADE_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_UPGRADE;
        } else if (action.equals(PolicyAdminConstants.RESTORE_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_RESTORE;
        } else if (action.equals(PolicyAdminConstants.BACKUP_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_BACKUP;
        } else if (action.equals(PolicyAdminConstants.LOG_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_LOG;
        } else if (action.equals(PolicyAdminConstants.RECORD_ROLE_ACTION)) {
            return PolicyManagementExceptionCodes.ACCESS_DENY_TO_RECORD;
        } 

        return 0;
    }

    public static String getAssetAdminUpdatePEMXPath(Element actionEL, String dbObjectName,  MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        
    	String serviceName = request.getServiceName();
        if (request.isSuperUser()) {
            return null;
        }

        List<String> roleObject = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);        
        
        String pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObject, dbObjectName,PolicyAdminConstants.MODIFY_ROLE_ACTION,  request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

        if (((pemXPath != null) && (pemXPath.length() != 0))) {
            pemXPath = PEMUtils.getPEMXPath(pemXPath, roleObject,dbObjectName, serviceName, request, schema, log);

            if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
                throw new MRException(AMExceptionCodes.ACCESS_DENY_TO_UPDATE, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
            }

            Element OperationData = MRXMLUtils.getFirstChildXpath(actionEL, "OperationData");

            Element groupsEL = null;

            if (roleObject.size() == 1 && roleObject.contains(AMConst.Device)) {
                groupsEL = MRXMLUtils.getElementXpath(OperationData, AMConst.ASSER_RESOURCE_NID);
            } else if (dbObjectName.equals(AMConst.User) || dbObjectName.equals(AMConst.DISPLAY_PROFILE)) {
                groupsEL = MRXMLUtils.getElementXpath(OperationData, "Groups");
            } else if (dbObjectName.equals(AMConst.MEDIA_ROOM)) {
                groupsEL = MRXMLUtils.getElementXpath(OperationData, "data/Groups");
            } else {
                groupsEL = MRXMLUtils.getElementXpath(OperationData, "Info/Groups");
            }

            if (groupsEL != null) {
                boolean pemCheck = false;
                String[] arrXPath = { pemXPath };
                pemCheck = IpvsDBUtils.validateXpath(OperationData, arrXPath, request, schema, log);

                if (!pemCheck) {
                    throw new MRException(AMExceptionCodes.ACCESS_DENY_TO_UPDATE, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
                }
            }
        }

        return pemXPath;
    }

    public static Map<String, String> getPEMXPathListForReadQuery(String query, MRRequest request, MRDB db, MRLog log, XMPPI xmpp,
            ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory) throws MRException, Exception {
            
        	Map<String, String> pemXpathList = getObjectsFromReadQuery(query);
            if (request.isSuperUser()) {
                return pemXpathList;
            }
           
            String dbObjectName = null;
            String pemXPath = null;
            String serviceName = null;

            //userGroupsEl = DBServiceUtils.getUser(request, db, log);
           // resourceGroupsEl = DBServiceUtils.getResource(request, db, log);
         
            for (Iterator<String> iterator = pemXpathList.keySet().iterator(); iterator.hasNext();) {
                dbObjectName = iterator.next();
                pemXPath = null;
                serviceName = MRConfig.getObjectServiceName(dbObjectName);
                List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log); 
                if (!AMConst.SERVICE_RESOURCE.equalsIgnoreCase(dbObjectName) && !PolicyAdminConstants.GROUPTAGDEF_OBJECTTYPE.equalsIgnoreCase(dbObjectName)) {
                	pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObjectList,dbObjectName, PolicyAdminConstants.READ_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                }
                validateReadQueryPemXpath(pemXpathList, pemXPath, roleObjectList, dbObjectName, serviceName, request, db, log);
            }
            return pemXpathList;
    }
    
    public static String checkPolicyPermissions(String action,String dbObject, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
	
	    if (request.isSuperUser()) {
	        return "";
	    }	    
	    List<String> roleObject = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObject, log);        
        String pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObject, dbObject,action, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        if (((pemXPath == null) || pemXPath.trim().equals("null"))) {
        	throw new MRException(getExceptionCode(action), MRXMLUtils.generateErrorXML("dbobjecttype", dbObject));
        }
        return pemXPath;
    }
    
    public static Map<String, String> getObjectsFromReadQuery(String query) throws Exception{
    	Map<String, String> pemXpathList = new Hashtable<String, String>(); 
        
        int startPos = 0;
        startPos = MRDBHelper.getParsedObjectType(query, startPos, pemXpathList);

        if (startPos != -1) {
            startPos = 0;

            while (startPos != -1) {
                startPos = MRDBHelper.getParsedObjectType(query, startPos, pemXpathList);
            }
        }
        return pemXpathList;
    }
    
    public static void validateReadQueryPemXpath(Map<String, String> pemXpathList, String pemXPath, List<String> roleObjectList, 
    		String dbObjectName, String serviceName, MRRequest request, MRDB db, MRLog log) throws MRException, Exception{
    	if (((pemXPath != null) && (pemXPath.length() != 0))) { 
            pemXPath = PEMUtils.getPEMXPath(pemXPath,roleObjectList, dbObjectName, serviceName, request, db, log);

            if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
                throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_READ,
                    MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
            } else {
                pemXpathList.put(dbObjectName, pemXPath);
            }
        }
    }
    
    
    public static String validateReadPemXpath( String pemXPath, List<String> roleObjectList, 
    		String dbObjectName, String serviceName, MRRequest request, MRDB db, MRLog log) throws MRException, Exception{
    	pemXPath = PEMUtils.getPEMXPath(pemXPath,roleObjectList, dbObjectName, serviceName, request, db, log);

    	if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
    		throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_READ,
    				MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
    	} 
    	return pemXPath;
    }
    
    public static void validateAddPEMXPath(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName, Element operationData, String dbObjectName) throws MRException, Exception
    {    	    	
    	if (!request.isSuperUser()) {	
    		    	
    		List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);
    	
    		String pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObjectList,dbObjectName, PolicyAdminConstants.ADD_ROLE_ACTION,  request, xmpp, log, schema, resourceFactory,
    				serviceAgentFactory);

    		//System.out.println("XPATH ----->"+pemXPath);
    		if (((pemXPath != null) && (pemXPath.length() != 0))) {
    			pemXPath = PEMUtils.getPEMXPath(pemXPath, roleObjectList,dbObjectName, serviceName, request, schema, log);

    			if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
    				throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_ADD, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
    			}
               
    			if(serviceName.equals(ServiceClient.ASSETADMINSERVICE))
    				operationData = validateAddOperationdata(roleObjectList,dbObjectName, operationData);
    			
    			if ((operationData != null)) {
    				boolean pemCheck = false;
    				String[] arrXPath = { pemXPath };
    				pemCheck = IpvsDBUtils.validateXpath(operationData, arrXPath, request, schema, log);

    				if (!pemCheck) {
    					throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_ADD, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
    				}            
    			}
    		}    	
    	}
    }
    
    public static Element validateAddOperationdata(List<String> roleObject,String dbObjectName,
			Element operationEl) throws MRException, Exception {
		
		if (roleObject.size()==1 && roleObject.contains(AMConst.Device)) {
			return  MRXMLUtils.getElementXpath(operationEl, AMConst.ASSER_RESOURCE_NID);
		} 
		 
		return operationEl;
	}
    
    public static String validateRepalcePEMXPath(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory, String serviceName, Element actionEL, Element operationData, String dbObjectName) throws MRException, Exception {
		
    	String pemXPath = "";
    	
    	if (!request.isSuperUser())      
    	{    		    	
    		if(serviceName.equals(ServiceClient.ASSETADMINSERVICE)){
    			return PolicyAdminServiceUtils.getAssetAdminUpdatePEMXPath(actionEL, dbObjectName, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    		}
		
    		List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);
		
    		pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObjectList,dbObjectName, PolicyAdminConstants.MODIFY_ROLE_ACTION,  request, xmpp, log, schema, resourceFactory,
                serviceAgentFactory);

    		if (((pemXPath != null) && (pemXPath.length() != 0))) {
    			pemXPath = PEMUtils.getPEMXPath(pemXPath, roleObjectList, dbObjectName, serviceName, request, schema, log);

    			if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
    				throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_UPDATE, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
    			}
            
    			if (operationData != null) {
    				boolean pemCheck = false;
    				String[] arrXPath = { pemXPath };
    				pemCheck = IpvsDBUtils.validateXpath(operationData, arrXPath, request, schema, log);

    				if (!pemCheck) {
    					throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_UPDATE, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
    				}
    			}
    		}
    	}
    	
		return pemXPath;
	}
    
    public static String validateDeletePEMXPath(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory, String serviceName, String dbObjectName) throws MRException, Exception
    {
    	String pemXPath = ""; 
    	
    	if (!request.isSuperUser()) {
        	List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);
            
            pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObjectList,dbObjectName, PolicyAdminConstants.DELETE_ROLE_ACTION,  request, xmpp, log, schema, resourceFactory,
                    serviceAgentFactory);

            //System.out.println(pemXPath);
            if (((pemXPath != null) && (pemXPath.length() != 0))) {
               pemXPath = PEMUtils.getPEMXPath(pemXPath,roleObjectList, dbObjectName, serviceName, request, schema, log);

                if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
                    throw new MRException(PolicyManagementExceptionCodes.ACCESS_DENY_TO_DELETE, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
                }
            }
        }
    	
    	return pemXPath;
    }

	public static String getTargetXPathForGivenPermissions(Document policiesResponseDoc,List<String> roleObjectList, String dbObjectType, String action,
			String wherePermissions,  MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			ServiceAgentFactoryI serviceAgentFactory,MRResourceFactoryI resourceFactory)  throws MRException,Exception {
	
    	StringTokenizer st = new StringTokenizer(wherePermissions, ",");
    	StringBuffer policyXpathbuffer = new StringBuffer();
    	String delim = "";
    	while (st.hasMoreTokens()) {
    		String token = st.nextToken();
    		token = token.trim();

    		for(String roleObject: roleObjectList) {

    			if(!roleObject.equals(AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE) && token.equalsIgnoreCase(PolicyAdminConstants.VIEW_FROM_ROLE_ACTION)){
    				continue;
    			}

    			if(!roleObject.equals(AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE) && token.equalsIgnoreCase(PolicyAdminConstants.STREAM_TO_ROLE_ACTION)) {
    				continue;
    			}

    			policyXpathbuffer.append(" " + delim +" ("+PolicyAdminConstants.ROLE_OBJECT+"='"+roleObject+"' and " +
    					"translate("+PolicyAdminConstants.ROLE_ACTION+",'abcdefghijklmnopqrstuvwxyz','ABCDEFGHIJKLMNOPQRSTUVWXYZ')='" + token.toUpperCase() +"')" ); 
    			delim = " or ";
    		}
    	}
    	List<?> matchingPoliciesList = null;
    	if(!policyXpathbuffer.toString().isEmpty()) {
			String roleXpath  =  "["+policyXpathbuffer.toString()+"]";
			matchingPoliciesList = MRXMLUtils.getListXpath(policiesResponseDoc, "//"+PolicyManager.POLICY_RULE + "[.//*"+roleXpath+"]");
		}
    	
    	if(matchingPoliciesList == null || matchingPoliciesList.isEmpty()) {
    		return PolicyAdminConstants.NULL;
    	}
    	
    	return TargetObjectLib.getTargetXpath(TargetObjectLib.createRequestDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory), matchingPoliciesList);
    
       	
	}
}
