package com.ipvs.sessionservice.resource.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;

public class SetPortStreamURLRequestHandler extends SetPortBaseRequestHandler {
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetPortStreamURLRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
		     				
		Element requestEl = requestObject.getRequestElement();			
		String streamURL = MRXMLUtils.getAttributeValue(requestEl, SessionServiceResourceConst.STREAM_URL_ATTRIBUTE, "");
				
		// Get port objectXML doc 
		Document objectXMLDoc = this.getPortObjectXMLDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		String parentElXPath = "//" + SessionServiceResourceConst.MEDIA_STREAM_URL_ELEMENT;
		
		//validate SDP for RTP stream type for this port
		List<String> streamTypeList = MRXMLUtils.getValuesXpath(objectXMLDoc, "//StreamTypeList[StreamType ='RTP']") ;
		if(!streamURL.isEmpty() && (streamTypeList!= null && !streamTypeList.isEmpty())){
			SDPUtils.parseSDP_JainParser(streamURL, log); //Exception on invalid SDP
		}
		
		MRXMLUtils.setValueXpath(objectXMLDoc, parentElXPath, streamURL);
		
		// Update		
		log.addInfo("SetPortStreamURLRequestHandler::Update element in db", MRLog.OK, MRLog.NOTIFY);		
		UpdateObjectUtils.updateElement(request, xmpp, log, db, requestObject.getObjectType(), requestObject.getNID(), objectXMLDoc, parentElXPath);
		        
		log.addInfo("SetPortStreamURLRequestHandler::handleResponse", MRLog.OK, MRLog.NOTIFY);				
		
		Document responseDoc = generatePortUpdateNotifications(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		
		return responseDoc; 
        
	}
	
}
