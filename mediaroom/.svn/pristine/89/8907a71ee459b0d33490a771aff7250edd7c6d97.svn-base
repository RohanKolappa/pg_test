package com.ipvs.sessionservice.handler.base;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.features.FeatureBase;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.window.handler.DisplayUtils;
import com.ipvs.utils.FileUtils;

public abstract class PortBaseRequestHandler extends AsyncBaseRequestHandler {

	
	public static Document executeQuery(MRRequest request, MRDB db, MRLog log, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String whereXPath, String queryName, String deviceElementName ) throws Exception {
		long dbtimeout = MRXMLDBUtils.TIME_OUT;
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + queryName + SessionServiceConst.XQUERYXML;
        String objectQuery = FileUtils.readFileResource(xqueryFileName, request.getClass());
		String query = objectQuery; 
        query = query.replace(SessionServiceConst.XPATHAND, whereXPath);
        if(deviceElementName!= null) {
        	query = query.replace(SessionServiceConst.ELEMENT, deviceElementName);
        }
        Map<String, String> pemXpathList = PolicyAdminServiceUtils.getPEMXPathListForReadQuery(query, request, db, log, xmpp,
                serviceAgentFactory, resourceFactory);
        String queryResult = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, pemXpathList, null);
        
        if ((queryResult == null) || queryResult.isEmpty()) {
            return null;
        }
        return MRXMLUtils.stringToDocument(queryResult);
	}
	
	
	public FeatureBase getSupportedFeature(Element portElement) throws Exception{
    	Element featureListElement = MRXMLUtils.getElementXpath(portElement, "FeatureList");
		if(featureListElement != null) {
			List<?> featureList = MRXMLUtils.getChildren(featureListElement);
			for(int i=0;featureList!=null && i<featureList.size();i++) {
				String feature = ((Element)featureList.get(i)).attributeValue("Name");
				if(feature== null || feature.isEmpty()) {
					continue;
				}
				return DisplayUtils.getSupportedFeature(feature);
			}
		}
		return null;
	}
	
	
	public static Element generateExceptionElement(int code, String description) {
		try {
			Element exceptionElement = MRXMLUtils.stringToElement("<Exception />");
			Element codeElement = MRXMLUtils.stringToElement("<Code />");
			Element descritpionElement = MRXMLUtils.stringToElement("<Description />");
			codeElement.setText(String.valueOf(code));
			descritpionElement.setText(description);
			exceptionElement.add(codeElement);
			exceptionElement.add(descritpionElement);
			return exceptionElement;
		}catch(Exception e) {
			e.printStackTrace();
			return null;
		}
	}
	
	
	public static Document getPortDoc (MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject) throws Exception{ 
		
		String nid = requestObject.getNID();
		String title = requestObject.getAttributeValue();
		
		// String objectType = SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE;
		String objectType = "#OBJ_"+ SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE + "_OBJ#" + " | " 
         +  "#OBJ_"+ SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE + "_OBJ#"; 
        
		List<String>listOfXPaths = new ArrayList<String>();
		requestObject.generateXPathBasedOnAttributeType(listOfXPaths, objectType, SessionServiceConst.WHERE_NID, nid, 
				SessionServiceConst.NID_CONST);
		requestObject.generateXPathBasedOnAttributeType(listOfXPaths, objectType, requestObject.getAttributeType(), title, 
				requestObject.getAttributeConstant());
		
		String whereXPath = GetObjectUtils.generateWhereXpathFromList(listOfXPaths, "and", objectType);
		Document resultDoc = executeQuery(request, db, log, xmpp, serviceAgentFactory, resourceFactory, whereXPath, "GetPortDisplayConfig", null);
		// System.out.println("PortBaseRequestHandler:getPortDoc doc=" + MRXMLUtils.documentToString(resultDoc));
		return resultDoc;
	}	
	
	public static Document getDeviceDoc (MRRequest request, XMPPI xmpp, MRLog log,MRDB db, 
			MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, 
			Element portElement, String deviceElementName, 
			String attributeForXPathGeneration, String objectType) throws Exception{ 
		
		String nid = portElement.attributeValue(SessionServiceConst.NID);
		String whereNIDXPath = "//" + objectType + SessionServiceConst.NID + "[NIDValue = '"+nid+"']";
	  	String selectXML = "<Device>" +"{root($VAR)/*/@NID}" +
    	"<Config>{$VAR"+deviceElementName+"}</Config>" +
    	"</Device>";
		
	    // Document deviceResultDoc = executeQuery(request, db, log, xmpp, serviceAgentFactory, resourceFactory, whereNIDXPath,"PortWindow", deviceElementName);
		Element responseElement = AssetAdminServiceUtils.getChildBasedOnSelectXML(SessionServiceConst.DEVICE, selectXML, whereNIDXPath, request, db, log, xmpp); 
			
		return MRXMLUtils.elementToDocument(responseElement);
	}
	
	
}
