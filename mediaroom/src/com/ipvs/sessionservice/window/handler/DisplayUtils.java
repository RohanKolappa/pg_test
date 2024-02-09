package com.ipvs.sessionservice.window.handler;

import java.util.HashMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.features.FeatureBase;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class DisplayUtils {

	public static FeatureBase getSupportedFeature(String feature)
	throws Exception {
		String packageName = "com.ipvs.sessionservice.features";
		String className = packageName + "." + feature;
		return (FeatureBase) Class.forName(className).newInstance();
	}


	public static void queueDisplayNotification(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,ServiceAgentFactoryI serviceAgentFactory, String xpath,String deviceNID,Element dataEl,String featureName, String portID,String notificationType,boolean addContent) throws Exception {

		if(portID == null) {
			String regex = "../PortID\\s*=\\s*(.*?)]";
			Matcher matcher = Pattern.compile(regex).matcher(xpath);
			if(matcher.find()) {
				portID= matcher.group(1);
			}
		}
		
		//find the feature name from xpath if input feature name is empty
		if(featureName == null ){
			
			String regex = "\\/(.*)\\/(.*)\\/(.*)\\/(.*?)\\[../PortID";
			int index =4; 
			if(notificationType.equals(SessionServiceWindowConst.DISPLAY_CONFIG)) {
				index = 1;
				regex = "\\/\\/(.*?)\\[../PortID";
			}
			
			Matcher matcher1 = Pattern.compile(regex).matcher(xpath);

			if(matcher1.find()) {
				featureName = matcher1.group(index); 
				featureName = featureName.replace("Status", "Config");
			}
		}
		
		if(featureName == null) {
			return;
		}		
			
		String portNID = DisplayUtils.getPortResourceNIDFromDevice(request, log, db, xmpp, deviceNID, portID);
		
		//this should not happened in regular case. only can happened before sending notification from admin ui some one deletes the device
		if(portNID.isEmpty()) {
			return;
		}			
		
		String elementName = SessionServiceConst.PORT + notificationType;
		log.addInfo("DisplayStatusUtils:Queue DisplayStatus notification to subscribers. FeatureName :="  +featureName + " portID:="+portID + " portNID :="+portNID,MRLog.INFO,MRLog.NOTIFY );
		FeatureBase featureObject = getSupportedFeature(featureName);
		Element notificationDisplayEl = MRXMLUtils.newElement(elementName);
		Element notificationDisplayEl1 = null;
		if(notificationType.equals(SessionServiceWindowConst.DISPLAY_CONFIG)) {
			notificationDisplayEl1 =  featureObject.getDisplayConfigElement(dataEl, log);
		} else {
			notificationDisplayEl1 =  featureObject.getDisplayStatusElement(dataEl, log);
		}
		
		if(notificationDisplayEl1 == null) {
			return;
		}	
		if(addContent) {			
			MRXMLUtils.replaceContent(notificationDisplayEl, notificationDisplayEl1);
		} else {
			MRXMLUtils.addElement(notificationDisplayEl,notificationDisplayEl1);
		}
	
		AppServerAgentUtils.getResourceManager(serviceAgentFactory).queueDisplayNotification(xmpp, request, log, db, SessionServiceConst.PORT, 
				notificationType , portNID, notificationDisplayEl);

	}
	
	
	public static String getPortResourceNIDFromDevice(MRRequestI request, MRLog log, MRDB db, XMPPI xmpp,String deviceNID,String portID) throws Exception {
    	String query = " let $deviceNID := '_DEVICE_NID_'" +"" +
    			"  	\n	    let $portId := '_PORT_ID_'" + 
    			"	\n		let $deviceDoc :=   doc(concat('/db/ipvs.default.', "+ 
    			"   \n                         substring-before( substring-after( $deviceNID, 'default.') , '.'  ) , '/', " + 
    			"   \n 							substring-after( substring-after( $deviceNID, 'default.'),'.')))/*[@NID=$deviceNID] "+ 
    			"   \n 		let $portNIDList := data($deviceDoc//NIDValue) "+
    			"   \n      let $portDoc := for $port in collection('ipvs.default.mediastreamioportresourcelist')/*    | collection('ipvs.default.mediastreamdstportresourcelist')/*" +
    			"   \n     						 where $port//@NID = $portNIDList 	and $port//DeviceElementID=$portId "+
    			"   \n      				return $port "+ 
    			"   \n		return $portDoc" ;
    	
    	 query  = query.replace("_DEVICE_NID_", deviceNID);
    	 query  = query.replace("_PORT_ID_", portID);
    	 String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
    	 if(responseXML ==null || responseXML.isEmpty()) {
    		 return "";
    	 }
    	 
    	 return MRXMLUtils.getAttributeValue(MRXMLUtils.stringToDocument(responseXML).getRootElement(), "NID","");
    }

}
