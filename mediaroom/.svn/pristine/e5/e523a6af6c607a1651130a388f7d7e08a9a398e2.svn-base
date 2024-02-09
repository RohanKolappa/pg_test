package com.ipvs.sessionservice.service.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.appserver.ServiceConfigManager;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.utils.Utils;

public class SetMulticastIPAddressPoolRequestHandler implements MRRequestHandlerI {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetMulticastIPAddressRangeRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	
    	Document requestDoc = request.getMessage().getDocument();
    	
    	String requestElementXpath = "//SetMulticastIPAddressPool";
		boolean enable = Utils.getBooleanValue(MRXMLUtils.getAttributeValueXpath(requestDoc, requestElementXpath, "enable", "false"),false);
    	String rangeStart = MRXMLUtils.getAttributeValueXpath(requestDoc, requestElementXpath, "from", "226.1.1.1");
    	String ranageEnd = MRXMLUtils.getAttributeValueXpath(requestDoc, requestElementXpath, "to", "226.1.1.255");
    	
    	ServiceConfigManager serviceConfigMgr = AppServerAgentUtils.getServiceConfigManager(serviceAgentFactory);
    	String serviceConfigNID = serviceConfigMgr.getServiceConfigNID();
    	Element serviceConfigEl = serviceConfigMgr.getServiceConfigElement();
    	Element multicastIPAddressPoolEl = MRXMLUtils.getElementXpath(serviceConfigEl, ".//MulticastIPAddressPool");
    	if (enable){
    		MRXMLUtils.setAttributeValue(multicastIPAddressPoolEl, "enable", "true");
    		MRXMLUtils.setAttributeValue(multicastIPAddressPoolEl, "start", rangeStart);
    		MRXMLUtils.setAttributeValue(multicastIPAddressPoolEl, "end", ranageEnd);
		} else {
			MRXMLUtils.setAttributeValue(multicastIPAddressPoolEl, "enable", "false");
    		MRXMLUtils.setAttributeValue(multicastIPAddressPoolEl, "start", "226.1.1.1"); 
    		MRXMLUtils.setAttributeValue(multicastIPAddressPoolEl, "end", "226.1.1.255");
			Element updateEl = MRXMLUtils.newElement("UsedIPAddressList");
			MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(multicastIPAddressPoolEl, ".//UsedIPAddressList"), updateEl );
		}
    	MRXMLDBUtils.updateElement(request, log, db, serviceConfigNID, serviceConfigEl);
    	serviceConfigMgr.resetServiceConfigNetworkResources(multicastIPAddressPoolEl);
    	/**
    	 * 1. update  v2d_lan profile to do multicast from admin ui. (note you don't see video in devlopernetwork but make sure session is showing streaming)
    	 * 2. on the SC tail -f /var/log/messages | grep ServiceConfig 
    	 * 3. start connecting to the source. observe on the box it will tell you allocating some ipaddress
    	 * 4. now stop stream should see it releasing the ipaddress if there is no more streams connected (For auto start one stream already running so it don't relase the ipaddress. 
    	 * 			for each connection check in the database used pool information is updating)
    	 * 5. Now connect to the same source again. 
    	 * 6. Run your script which will update the pool range 100-200 like that (This should take care of resetting the stream)
    	 * 7. now stop stream you will see some log message saying releasing the ipaddress but it should not put ipaddress back into the cache because its not in the range
    	 * 8. now start connecting to the source again this should use new ipaddress pool.
    	 * 
    	 * repeat 6 steps for different cases. like strink the range expand the range verify if ipaddress range is not in range its not adding back. if it is in the range put in the available list.
    	 *  
    	 */
    	return request.createResponseUsingRequestRootName(null);
	}

	
	@Override 
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
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

}