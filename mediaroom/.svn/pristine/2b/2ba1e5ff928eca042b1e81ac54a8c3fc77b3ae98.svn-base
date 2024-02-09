package com.ipvs.mediaroomservice.handler;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;


public class CleanMonitorAgentStreamsRequestHandler implements MRRequestHandlerI {

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
				
		log.addTraceBegin(request);

		int authType = MediaRoomObject.STREAM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;

		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);

		return auth;

	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		
		Element data = MediaRoomServiceUtils.cloneDataElement(request);
		Document responseDoc = request.createResponseData();
		
        String roomNID = MRXMLUtils.getValue(data,"roomNID","");
        String monitorAgentJID = MRXMLUtils.getValue(data,"MonitorAgentJID","");
         
        Document roomDoc = null;
        try {
        	roomDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
        }
        catch(Exception e) {
        	log.addInfo("CleanMonitorAgentStreamsRequestHandler:=" + request.getUserJID() + "/" + roomNID + " CleanMonitorAgentStream Exception=" + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
        	MRXMLUtils.setValue(responseDoc.getRootElement(), "ROOM NOT FOUND");
        	return responseDoc;
        }
        
        String roomOwnerJID = MRXMLUtils.getValueXpath(roomDoc, "//data/roomOwnerJID", "");
        if(roomOwnerJID.equals(monitorAgentJID)) {
        	MediaRoomServiceUtils.deleteMediaRoom(xmpp, db, log, profileFactory, resourceFactory, roomNID, request.getUserJID());
        } else {        
	        String action = MediaRoomServiceConst.DELETE;
	        String requestName = request.getRequestName();
	        String xpath  = "//MediaSource[data/monitorAgentJID='"+ monitorAgentJID + "']";	    
	    	
	        List<?> mediaSourceList = MRXMLUtils.getListXpath(roomDoc, xpath);
	    	for(Iterator<?> i = mediaSourceList.iterator(); i.hasNext();) {
	    		Element mediaSourceElement = (Element)i.next();
	    		String mediaSourceNID = MRXMLUtils.getAttributeValue(mediaSourceElement,"NID");
	    		xpath = "//Stream[data/mediaSourceNID='" + mediaSourceNID + "']";
	    		
	    		List<?> streamList = MRXMLUtils.getListXpath(roomDoc, xpath);
	    		for(Iterator<?> j = streamList.iterator(); j.hasNext();) {
	        		Element streamElement = (Element)j.next();
	        		String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
	        		String profileNID = streamElement.element("data").element("profileNID").getText();
	                Element profileXML = streamElement.element("data").element("profileXML");
	                ConnectionUtils.addStreamAction(request, xmpp, log, db, resourceFactory, profileFactory, requestName, roomNID, streamNID, profileNID, profileXML, action);
	        	}
			}
        }    	      		
		return responseDoc;
	}

}
