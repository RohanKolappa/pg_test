package com.ipvs.common.db;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;


public class DBServiceUtils {
     

    
	public static Element getUserAndResource(MRRequest request, String NID, String groupType, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
	throws Exception {

		com.ipvs.common.XMPPTransaction.startBlock("GetUserAndResourceGroups");
		Element userEL = null;
		// Element resourceEL = MRXMLDBUtils.readElement(1, null, null, NID, -1, null, null, request, db, log);
		Document resourceDoc = MRXMLDBUtils.readElement(request, log, db, NID);
		Element resourceEL = (Element)resourceDoc.getRootElement().clone();

		Element groups = MRXMLUtils.stringToElement("<" + groupType + "/>");

		userEL = getUserFromContactJID(request, resourceEL, db, log);
		if(userEL!=null) {
		    MRXMLUtils.addElement(groups,(Element)userEL.detach());
		}	else {
			MRXMLUtils.addElement(groups,AMConst.User);    			
		}
		MRXMLUtils.addElement(groups,(Element)resourceEL.detach());
		com.ipvs.common.XMPPTransaction.endBlock("GetUserAndResourceGroups");
		return groups;
	}
    
	public static Element getUserAndResource(MRRequest request, String fileNID, String portNID, String resourceXPathParent, String groupType, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
	throws Exception {

		Element userEL = null;
		Element resourceEL = MRXMLDBUtils.readChildElement(2, portNID, ServiceClient.ASSETADMINSERVICE, resourceXPathParent, "//" + resourceXPathParent, log, request, db);
		Element groups = MRXMLUtils.stringToElement("<" + groupType + "/>");
		userEL = getUserFromContactJID(request, resourceEL, db, log);	
		MRXMLDBUtils.readElement(-1, null, null, fileNID, resourceEL, request, log, db);
		
		if(userEL!=null) {
		    MRXMLUtils.addElement(groups,(Element)userEL.detach());
		} else {
			MRXMLUtils.addElement(groups,AMConst.User);    			
		}
		MRXMLUtils.addElement(groups,(Element)resourceEL.detach());
		return groups;
	}  

    public static String getAssetResourceNID(String resourceName, MRRequest request, MRLog log, MRDB db) throws Exception {
    	Element resourceElement = DBServiceUtils.getAssetResource(resourceName, request, log, db);
    	return MRXMLUtils.getAttributeValue(resourceElement,"NID");
    }
    
    public static Element getAssetResource(String resourceName, MRRequest request, MRLog log, MRDB db) throws Exception {
		Element resourceElement = MRXMLDBUtils.findChildElement_JumpNID(2, ServiceClient.ASSETADMINSERVICE, "AssetResource", "//AssetResource", "//AssetResource", "//AssetResource/Info[Title='" + resourceName + "']", log, request, db);
		return resourceElement;
    }    
    
    public static Element getUser(MRRequest request, MRDB schema, MRLog log) throws MRException, Exception {
		String userJID = request.getUserJID();
		int serverindex = userJID.indexOf("@");		
		if(serverindex == -1) {
			return null;
		}
		String username = userJID.substring(0, serverindex);
		String serviceName = ServiceClient.ASSETADMINSERVICE;
		Element userEL =  MRXMLDBUtils.findChildElement_JumpNID(2, serviceName, "User", "//User", "//User", "//User[Username='" + username + "']", log, request, schema);
		if(userEL ==null){
			return null;
		}
		return (Element) userEL.clone();		
	}
	
    public static Element getUserFromContactJID(MRRequest request, Element resourceEL, MRDB schema, MRLog log) throws MRException, Exception {
    	
		Element userEL = null;
		String userJID = resourceEL.element("Contact").elementText("OwnerUserJID");
	    int serverindex = userJID.indexOf("@");
	    if(serverindex == -1) {
	    	return MRXMLUtils.addElement(AMConst.User);   
	    }
	    String username = userJID.substring(0, serverindex);	   
		userEL = MRXMLDBUtils.findChildElement_JumpNID(2, request.getServiceName(), "User", "//User", "//User", "//User[Username='" + username + "']", log, request, schema);
		if(userEL ==null){
			return MRXMLUtils.addElement(AMConst.User);   
		}
		return (Element)userEL.clone();	
	}
	
	public static Element getResource(MRRequest request, MRDB schema, MRLog log) throws MRException, Exception {
		String serviceName = ServiceClient.ASSETADMINSERVICE;
		Element resourceElement = null;
		String userJID = request.getUserJID();
		int resourceindex = userJID.indexOf("/");
		if(resourceindex  == -1) {
			return null;
		}

		String resourceName = userJID.substring(resourceindex + 1);
		
		resourceElement = MRXMLDBUtils.findChildElement_JumpNID(2, serviceName, "AssetResource", "//AssetResource", "//Info", "//AssetResource/Info[Title='" + resourceName + "']", log, request, schema);		
		return resourceElement;
	}
	
	  public static Element getServiceConfigResource(String resourceName, MRRequest request, MRLog log, MRDB db) throws Exception {
			Element resourceElement = MRXMLDBUtils.findChildElement_JumpNID(2, ServiceClient.ASSETADMINSERVICE, "ServiceConfig", "//ServiceConfig", "", "//MultiCastIpAddressPool", log, request, db);
			return resourceElement;
	    }

    
}
