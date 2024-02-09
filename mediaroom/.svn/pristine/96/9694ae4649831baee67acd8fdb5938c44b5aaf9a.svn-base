package com.ipvs.sessionservice.resource.handler;

import java.util.HashMap;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class HATakeoverDeviceRequestHandler extends BaseDeviceRequestHandler {

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addInfo("HATakeoverDeviceRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		
		Element assetResourceInfoEl = getAssetResourceInHA(request, db, log, "standby");
		Element assetEL = assetResourceInfoEl.element(AMConst.AssetResource);
		String standByMSJID = MRXMLUtils.getValueXpath(assetEL, ".//"+ AMConst.OWNERUSERJID, "");
		String standByMSNID = MRXMLUtils.getAttributeValue(assetEL, AMConst.NID);
		String requestName = MRRequest.getRequestRootName(request.getRequestName());		
		Element requestEl = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + requestName);
		MRXMLUtils.setAttributeValue(requestEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, standByMSNID);
		
		MRRequest standByRequest = MRRequest.createRequest(standByMSJID, request.getLogLevel(), standByMSNID, request.getServiceName(), request.getRequestName(), request.getMessage(), request.getResourceGroup());
		sendAgentRequest(standByRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, "HATakeoverDeviceRequest");
		return request.createResponseData();
	}

	
	public Element getAssetResourceInHA(MRRequest request, MRDB db, MRLog log, String state) throws Exception {
		
		String query = 
    			"	\n		let $serviceResourceDoc := for $serviceResource in collection('/db/ipvs.default.serviceresourcelist')/*[(@parentNID='default.serviceresourcelist')]"+
    			"   \n 		return $serviceResource" +
    			"   \n 		let $resourceNID :=  data($serviceResourceDoc//HA/*[@state='"+ state +"']/@MSAssetResourceNID)" +
    			"   \n 		let $assetNID := $resourceNID" +
    			"   \n 		let $assetDoc :=  if($resourceNID !='') then" +
    			"   \n 			doc(concat('/db/ipvs.default.'," +
    			"   \n 			substring-before( substring-after( $assetNID, 'default.') , '.'  ) , '/'," +
    			"   \n 			substring-after( substring-after( $assetNID, 'default.'),'.')))/*[@NID=$assetNID]" +
    			"   \n 		else ''" +
    			"   \n 		return <AssetResourceInfo>{$assetDoc}</AssetResourceInfo>";
		
		String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
		if(responseXML ==null || responseXML.isEmpty()) {
   	 		throw new MRException(SessionServiceExceptionCodes.DEVICE_COMMAND_EXECUTION_FAILED, "Can not read asset resource");
   	 	}
   	 	return MRXMLUtils.stringToElement(responseXML);
	}
}
