package com.ipvs.sessionservice.resource.handler;


import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;


public class SetDeviceInfoRequestHandler extends BaseDeviceRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
    	log.addInfo("SetDeviceInfoRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.AssetResource);
    	
    	String password =  MRXMLUtils.getAttributeValue(requestObject.getRequestElement(), SessionServiceResourceConst.PASSWORD_ATTRIBUTE);
    	String applyTagsToPorts = MRXMLUtils.getAttributeValue(requestObject.getRequestElement(), SessionServiceResourceConst.APPLY_TAGS_TO_PORTS_ATTRIBUTE,SessionServiceConst.TRUE);
    	String description = MRXMLUtils.getAttributeValue(requestObject.getRequestElement(), SessionServiceConst.DESCRIPTION.toLowerCase());
    	Element groupsEL = MRXMLUtils.getElementXpath(requestObject.getRequestElement(), "./"+SessionServiceResourceConst.GROUPS_ELEMENT);
    	String devicePrivateData = MRXMLUtils.getValueXpath(requestObject.getRequestElement(), "./"+SessionServiceResourceConst.DEVICE_PRIVATE_DATA_ELEMENT,null);
        
    	if(password == null && description == null && groupsEL == null && devicePrivateData == null) {
    		throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid Input");
    	}
    	
    	//update asset resource 
    	Document assetResourceDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, "", "");
    	String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), SessionServiceConst.NID,"");
    	String parentElXPath = "//" + SessionServiceResourceConst.INFO_ELEMENT;
    	MRXMLUtils.setValueXpath(assetResourceDoc, SessionServiceResourceConst.DESCRIPTION_XPATH, description);
    	MRXMLUtils.setValueXpath(assetResourceDoc, SessionServiceResourceConst.PASSWORD_XPATH, password);
    	MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(assetResourceDoc, "//"+SessionServiceResourceConst.GROUPS_ELEMENT), groupsEL);
    	MRXMLUtils.setValueXpath(assetResourceDoc, "//"+SessionServiceResourceConst.ASSET_PRIVATE_DATA_ELEMENT, devicePrivateData);
    	UpdateObjectUtils.updateElement(request, xmpp, log, db, AMConst.AssetResource,assetResourceNID, assetResourceDoc, parentElXPath);
    	
    	//update port description and groups 
    	if(description !=null || (groupsEL!=null && applyTagsToPorts.endsWith(SessionServiceConst.TRUE))){
    		Document deviceDoc = AssetAdminServiceUtils.getDeviceResourcePointers(assetResourceNID, request, db, log, xmpp);
    		List<?> listOfPorts = AssetAdminServiceUtils.getListOfPorts(deviceDoc,assetResourceNID,request,log,db);
    		if(listOfPorts !=null && !listOfPorts.isEmpty() ){
    			//delete ports
    			for(Object  object:listOfPorts) {
    				Element nidEL = (Element) object;
    				String portResourceNID = MRXMLUtils.getValue(nidEL);
    				if (portResourceNID == null) {
    					continue;
    				}
    				String portElName =nidEL.getParent().getName();
    				String portType = AssetAdminServiceUtils.getPortResourceTypeFromDevice(portElName);
    				Document portDocument = MRXMLDBUtils.readElement(request, log, db, portResourceNID);
    				updatePortInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory,null , description, groupsEL, portDocument, portType, portResourceNID, applyTagsToPorts);
    			}
    		}
    	}
    	
    	//update policy cache if groups are updating 
		if(groupsEL !=null) {
			StreamPolicyUtils.handlePolicyChanged();
		}
		
    	log.addInfo("Update Device Info. Title:="+MRXMLUtils.getValueXpath(assetResourceDoc, "//"+SessionServiceResourceConst.TITLE, ""),MRLog.INFO,MRLog.NOTIFY);    	
    	return GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, 
        		SessionServiceConst.SELECTFORMAT_EXTENDED, GET_DEVICE_REQUEST_XML_PATH, GetDeviceRequestHandler.class.getName());
    	
    }
    
      
}