package com.ipvs.assetadminservice.impl;

import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.CustomValidation;
import com.ipvs.common.db.DBExceptionCodes;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.customvalidation.ValidatorPluginDelegate;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.policyadminservice.impl.PolicyManagementValidation;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.utils.PEMUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.resource.handler.DeleteFileRequestHandler;

public class BaseManagementRequestHandler implements EventLoggerI {
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    log.addTraceBegin(request);

    int authType = MediaRoomObject.ROOM_TYPEID;
    int role = MediaRoomPEM.VISITOR_ROLE;
    String token = "testCommonToken";
    String authority = "testAuthority";
    String roomPolicyNID = MRConst.NULL;
    String streamPolicyNID = MRConst.NULL;
    MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
    log.addTraceEnd(request);

    return auth;
	}
	
	public Document GetResponseBasedOnAction(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName)  throws MRException, Exception {{
	Document resDoc = MRXMLUtils.getDocument();

    try {
        Document reqDoc = request.getMessage().getDocument();
        String requestTag = null;
        if(serviceName.equals(ServiceClient.ASSETADMINSERVICE)){
        	requestTag = "AssetManagementRequestData";
        }
        if(serviceName.equals(ServiceClient.POLICYADMINSERVICE)){
        	requestTag = "PolicyManagementRequestData";
        }
        String dbObjectName = MRXMLUtils.getValueXpath(reqDoc, "//"+requestTag+"/ObjectType", "");

        @SuppressWarnings("unused")
        boolean bFailed = false;

        // add response resDoc
        Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
        MRXMLUtils.addElement(rootRes, "ObjectType", dbObjectName);

        // AMUtil
        MRDBHelper amUtil = new MRDBHelper();
        amUtil.doInit(serviceName, dbObjectName, resDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

        Element userGroups = null;
        Element resourceGroups = null;
        String pemXPath = null;

        // PEM check get userGroups and ResourceGroups
        if (!request.isSuperUser() ) {
            userGroups = DBServiceUtils.getUser(request, schema, log);
            resourceGroups = DBServiceUtils.getResource(request, schema, log);
        }

        // call actions in loop
        Element parentEL = MRXMLUtils.getElementXpath(reqDoc, "//"+requestTag);
        List<?> listEL = MRXMLUtils.getList(parentEL);

        for (Iterator<?> i = listEL.iterator(); i.hasNext();) {
            Element actionEL = (Element) i.next();
            String action = actionEL.getName();

            if (!action.equalsIgnoreCase("GetRequest") && dbObjectName.equals("MediaRoom")) {
                continue;
            }

            if (action.equalsIgnoreCase("ObjectType")) {
                // skip
            } else if (action.equalsIgnoreCase("ReadQueryRequest")) {
                handleReadQuery(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,serviceName,action,amUtil,actionEL,resourceGroups,userGroups,resDoc);
            } else if (action.equalsIgnoreCase("GetRequest")) {
                try {
                	handleGet(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,serviceName,amUtil,actionEL,resourceGroups,userGroups,pemXPath,dbObjectName,action,resDoc);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_GET_FAILED, ex.getMessage());
                }
            } else if (action.equalsIgnoreCase("ReplaceRequest")) {
                try {
                	handleReplace(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,serviceName,amUtil,actionEL,resourceGroups,userGroups,pemXPath,dbObjectName,action,resDoc);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_UPDATE_FAILED, ex.getMessage());
                }
            } else if (action.equalsIgnoreCase("AddRequest")) {
                try {
                	handleAdd(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,serviceName,amUtil,actionEL,resourceGroups,userGroups,pemXPath,dbObjectName,action,resDoc);
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_ADD_FAILED, ex.getMessage());
                }
            } else if (action.equalsIgnoreCase("DeleteRequest")) {
                try {
                	
                    if(dbObjectName.equals(AMConst.MEDIA_STREAM_FILE_RESOURCE)){
	                	//resDoc = MediaManagementUtils.handleDeleteMediaStreamFile(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,serviceName,amUtil,actionEL,resourceGroups,userGroups,pemXPath,dbObjectName,action,resDoc,reqDoc);
	                    String 	mediaStreamFileNID = MRXMLUtils.getValueXpath(reqDoc, "//NID", "");
	                    	
	                	Document responseDoc= new DeleteFileRequestHandler().handleDeleteMediaStreamFile(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, pemXPath,  mediaStreamFileNID,true,reqDoc,AMConst.MEDIA_DELETE_REQUEST);
	
	                	if(responseDoc == null) {
		               	    return null;
		               	}                	
	                	
                    } else {
                	  handleDelete(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,serviceName,amUtil,actionEL,resourceGroups,userGroups,pemXPath,dbObjectName,action,resDoc,reqDoc);
                    }
                	
                    
                } catch (MRException ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(ex.getCode(), ex.getData());
                } catch (Exception ex) {
                    bFailed = true;
                    ex.printStackTrace();
                    throw new MRException(DBExceptionCodes.DB_DELETE_FAILED, ex.getMessage());
                }
            }
        }

        amUtil.doUnInit();
        amUtil = null;
        log.addTraceEnd(request);
        resDoc.getRootElement().setName(request.getResponseDataName());

        // log.addInfo("AssetManagement-End");
        return resDoc;
    } catch (MRException ex) {
        ex.printStackTrace();
        throw new MRException(ex.getCode(), ex.getData());
    } catch (Exception ex) {
    	ex.printStackTrace();
        throw new MRException(AMExceptionCodes.SERVICE_ERROR, ex.getMessage());
    } finally {
    }
}
}
	

	private void handleReadQuery(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName, String action,MRDBHelper amUtil, Element actionEL, Element resourceGroups, Element userGroups, Document resDoc) throws MRException,Exception{
		Map<String, String> objectTypes = amUtil.parseObjectTypesInQuery(actionEL);
        boolean pemCheck = false;

        if (!request.isSuperUser()) {
            if (objectTypes == null) {
                pemCheck = true;
            } else {
                //todo: test 
            	for (Iterator<String> iterator = objectTypes.keySet().iterator(); iterator.hasNext();) {
            		String  dbObjectName = iterator.next();
            		String pemXPath = null;
            		List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);

            		if (!AMConst.SERVICE_RESOURCE.equalsIgnoreCase(dbObjectName) && !PolicyAdminConstants.GROUPTAGDEF_OBJECTTYPE.equalsIgnoreCase(dbObjectName)) {
            			pemXPath = PolicyAdminServiceUtils.getTargetXpath(roleObjectList,dbObjectName, PolicyAdminConstants.READ_ROLE_ACTION, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
            		}
            		if (((pemXPath != null) && (pemXPath.length() != 0))) {
            			pemXPath = PEMUtils.getPEMXPath(pemXPath,roleObjectList, dbObjectName, serviceName, request, schema, log);

            			if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
            				Element whereXPathElement = MRXMLUtils.getElementXpath(resDoc, "//Handler");
            				String objectWhereCountXML = getResposneXML_Empty(dbObjectName);
            				Element objEL = MRXMLUtils.stringToElement(objectWhereCountXML);
            				MRXMLUtils.replaceContentWithElement(whereXPathElement,objEL, false);
            				pemCheck = true;

            				break;
            			} else {
            				objectTypes.put(dbObjectName, pemXPath);
            			}
            		}
            	}
            }
        }

        if (!pemCheck) {
            com.ipvs.common.XMPPTransaction.startBlock("doReadQueryRequest");
            amUtil.doReadQueryRequest(actionEL, "ReadQueryResponse", objectTypes);
            com.ipvs.common.XMPPTransaction.endBlock("doReadQueryRequest");
        }
	}
	
	private void handleGet(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName, MRDBHelper amUtil, Element actionEL, Element resourceGroups, Element userGroups, String pemXPath, String dbObjectName, String action, Document resDoc) throws MRException,Exception{
		 //attempt to  read dbObjectName from NID
        if (dbObjectName.trim().equals("")) {
            dbObjectName = amUtil.getDbObjectName(actionEL, request, schema, log);
        }

        boolean bCustomValidation = customValidateReq("Get", serviceName, dbObjectName, null, actionEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

        if (!bCustomValidation) {
            // TBD : raise error
        }

        boolean pemCheck = false;

        if (!request.isSuperUser()) {
        	List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(dbObjectName, log);
        	
        	if (!AMConst.SERVICE_RESOURCE.equalsIgnoreCase(dbObjectName) && !PolicyAdminConstants.GROUPTAGDEF_OBJECTTYPE.equalsIgnoreCase(dbObjectName)) {
        		pemXPath = PolicyAdminServiceUtils.getTargetXpath( roleObjectList,dbObjectName,PolicyAdminConstants.READ_ROLE_ACTION,  request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
        	}

            if (((pemXPath != null) && (pemXPath.length() != 0))) {
                pemXPath = PEMUtils.getPEMXPath(pemXPath,roleObjectList, dbObjectName, serviceName, request, schema, log);

                if (((pemXPath == null) || (pemXPath.length() == 0) || pemXPath.trim().equals("null"))) {
                    Element whereXPathElement = MRXMLUtils.getElementXpath(resDoc, "//Handler");
                    String objectWhereCountXML = getResposneXML_Empty(dbObjectName);
                    Element objEL = MRXMLUtils.stringToElement(objectWhereCountXML);
                    MRXMLUtils.replaceContentWithElement(whereXPathElement, objEL);
                    pemCheck = true;
                }
            }
        }

        if (!pemCheck) {
            com.ipvs.common.XMPPTransaction.startBlock("doGet");
            amUtil.doGet(actionEL, "GetResponse", dbObjectName, pemXPath);
            com.ipvs.common.XMPPTransaction.endBlock("doGet");
        }
	}
	
	private void handleReplace(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName, MRDBHelper amUtil, Element actionEL, Element resourceGroups, Element userGroups, String pemXPath, String dbObjectName, String action, Document resDoc) throws MRException,Exception{
		 //attempt to  read dbObjectName from NID
        if (dbObjectName.trim().equals("")) {
            dbObjectName = amUtil.getDbObjectName(actionEL, request, schema, log);
        }

        boolean bCustomValidation = customValidateReq("Replace", serviceName, dbObjectName, null, actionEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

        if (!bCustomValidation) {
            // TBD : raise error
        }
	    	    
        Element operationData = MRXMLUtils.getFirstChildXpath(actionEL, "OperationData");
        pemXPath = PolicyAdminServiceUtils.validateRepalcePEMXPath(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, serviceName, actionEL, operationData, dbObjectName);

        amUtil.doUpdate(actionEL, "ReplaceResponse", dbObjectName, pemXPath);
        
        this.handlePolicyChanged(serviceName);
	}
	
	private void handleAdd(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName, MRDBHelper amUtil, Element actionEL, Element resourceGroups, Element userGroups, String pemXPath, String dbObjectName, String action, Document resDoc) throws MRException,Exception{
		//raise error if dbObjectName is empty 
        if (dbObjectName.trim().equals("")) {
            throw new MRException(AMExceptionCodes.OBJECTTYPE_NOT_PROVIED, actionEL.getName() + "   ObjectType not provided ");
        }

        boolean bCustomValidation = customValidateReq("Add", serviceName, dbObjectName, null, actionEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
        if (!bCustomValidation) {
            throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
        }

        Element operationData = MRXMLUtils.getFirstChildXpath(actionEL, "OperationData");               
        PolicyAdminServiceUtils.validateAddPEMXPath(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, serviceName, operationData, dbObjectName);        
                
        amUtil.doAdd(actionEL, "AddResponse", dbObjectName, pemXPath);

        //Invalidate the global cache on any modification in assets
        this.handlePolicyChanged(serviceName);

	}
	
	
	private void handleDelete(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String serviceName, MRDBHelper amUtil, Element actionEL, Element resourceGroups, Element userGroups, String pemXPath, String dbObjectName, String action, Document resDoc, Document reqDoc) throws MRException,Exception{
		 //attempt to  read dbObjectName from NID
        if (dbObjectName.trim().equals("")) {
            dbObjectName = amUtil.getDbObjectName(actionEL, request, schema, log);
        }
    
        pemXPath = PolicyAdminServiceUtils.validateDeletePEMXPath(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, serviceName, dbObjectName);
        
        boolean bCustomValidation = customValidateReq("Delete", serviceName, dbObjectName, null, actionEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

        if (!bCustomValidation) {
            throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, MRXMLUtils.generateErrorXML("dbobjecttype", dbObjectName));
        }
        
        this.validateDeleteRequest(request,dbObjectName,schema,log,xmpp, resourceFactory, serviceAgentFactory,actionEL,serviceName,pemXPath,reqDoc,resDoc,amUtil);
        if(request.getLogLevel() == MRLog.INFO) {
        	  MRDBHelper amGetUtil = new MRDBHelper();
        	  Document getResponseDoc = MRXMLUtils.getDocument();
        	  MRXMLUtils.addElement(getResponseDoc, "Handler");
        	  amGetUtil.doInit(serviceName, dbObjectName, getResponseDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
        	  Element getActionEL =(Element) actionEL.clone();
        	  getActionEL.setName("GetRequest");
        	  amGetUtil.doGet(getActionEL, "GetResponse", dbObjectName, pemXPath);
        	  request.setLoggingDocument(getResponseDoc);
        }
      
        amUtil.doDelete(actionEL, "DeleteResponse", dbObjectName, pemXPath);
        //Invalidate the global cache on any modification in assets
        this.handlePolicyChanged(serviceName);
	}
	
	
	
	private String getResposneXML_Empty(String dbObjectName) {
	        return "<Handler><ObjectType>" + dbObjectName + "</ObjectType><GetListObjectResponseData><ObjectCount>0</ObjectCount><ObjectLastWhereEndCount>0</ObjectLastWhereEndCount></GetListObjectResponseData><ResponseXML></ResponseXML></Handler>";
	    }
	
	public void  validateDeleteRequest(MRRequest request, String dbObjectName,
			MRDB schema, MRLog log, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element actionEL,
			String serviceName, String pemXPath, Document reqDoc,
			Document resDoc, MRDBHelper amUtil)
			throws MRException, Exception {
		 boolean pemFlag = true;
		 
//         if (((pemXPath != null) && (pemXPath.length() != 0))) {
//             pemFlag = PolicyManagementValidation.validatePEM(actionEL, pemXPath, dbObjectName, request, schema, log);
//         } else {
//             pemFlag = true;
//         }

         if (pemFlag) {
             if (dbObjectName.equalsIgnoreCase(PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE)) {
                 PolicyManagementValidation.validateStreamPolicy(actionEL, serviceName, request, schema, log);
             } else if (dbObjectName.equalsIgnoreCase(PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE)) {
                 PolicyManagementValidation.validateFilter(actionEL, serviceName, request, schema, log);
             } else if (dbObjectName.equalsIgnoreCase(PolicyAdminConstants.OBJECT_FILTER_OBJECTYPE)) {
                 PolicyManagementValidation.validateObjectFilter(actionEL, serviceName, request, schema, log);
             } 
         } 
    		
	}
		
	 
	 public void handlePolicyChanged(String serviceName) {
		 if(serviceName.equals(ServiceClient.ASSETADMINSERVICE))
			 StreamPolicyUtils.handlePolicyChanged();
			
		}
	 
	    public boolean customValidateReq(String action, String serviceName, String dbObjectName, ValidatorPluginDelegate defaultValidator, Element actionEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
	        throws Exception {
	        //@SuppressWarnings("unused")
	        boolean bCustomValidation = false;

	        /*if (bdisable_customValidateReq) {
	            return true;
	        }*/
	        try {
	            MRConfig pmConfig = MRConfig.getInstance(serviceName, dbObjectName, log);
	            CustomValidation customValidation = null;

	            try {
	                customValidation = pmConfig.getCustomValidation(log);
	            } catch (Exception ex) {
	                log.addInfo("ERROR Custom validtion xml not defiened for (serviceName, dbObjectName) (" + serviceName + "," + dbObjectName + ")", MRLog.ERROR, MRLog.ERROR);

	                //ex.printStackTrace();
	            }

	            // TODO: try different CustomValidationvalidation based on action
	            // THis is good for base_all base_part add/update only
	            if ((customValidation == null) || (customValidation.getDoc() == null)) {
	                return true;
	            } else {
	                //Element OperationData = MRXMLUtils.getFirstChildXpath(actionEL, "OperationData");
	                //if (OperationData != null) {
	                bCustomValidation = customValidation.customValidateActionReq(action, serviceName, dbObjectName, null, actionEL, customValidation.getDoc().getRootElement(), request, xmpp, log, schema, resourceFactory, serviceAgentFactory);

	                //}
	            }
	        } catch (MRException ex) {
	            bCustomValidation = false;
	            throw ex;
	        } catch (Exception ex) {
	            bCustomValidation = false;
	            log.addLog(MRLog.ERROR, 1, ex.getMessage());
	            throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, "Validation error "+ex.getMessage());
	        }

	        return bCustomValidation;
	    }

	    @Override
	    public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, String status, String level,String errorDescription) throws Exception {
	    	
	    	//If event already logged then set the flag to false;
	    	if(!request.logEvent()) {
	    		return null;
	    	}
	    	String dbObjectName = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), request.getRequestDataXPath()+"/ObjectType", "");
	    	Element actionEl = MRXMLUtils.getFirstChildXpath(request.getMessage().getDocument().getRootElement(), request.getRequestDataXPath());
	        String actionName = MRXMLUtils.getElementName(actionEl,"");
	        
	        if(actionName.equals(AMConst.DELETE_REQUEST)){
	        	String childxpath = MRXMLUtils.getValueXpath(actionEl, ".//ChildXPath", "");
	        	if(!childxpath.isEmpty()) {
	        		actionName = AMConst.REPLACE_REQUEST;
	        	}
	        	return AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), request.getMessage().getDocument(), request.getLoggingDocument(), request.getOperatorGroups(), status, level, errorDescription, dbObjectName, actionName);
	        }	        
	    	return AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(request,xmpp,log,db,status,level,errorDescription,dbObjectName,actionName);
	    }

}

