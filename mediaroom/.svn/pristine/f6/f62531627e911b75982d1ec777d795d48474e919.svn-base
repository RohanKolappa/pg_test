package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;


public class SetPortPVRInfoRequestHandler extends SetPortBaseRequestHandler {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetPortPVRInfoRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
				
		Element requestEl = requestObject.getRequestElement();
		
		String enable = MRXMLUtils.getAttributeValue(requestEl, SessionServiceResourceConst.ENABLE_ATTRIBUTE, SessionServiceConst.FALSE);
		String trimLength = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.TRIMLENGTH, MediaRoomServiceConst.DEFAULT_TRIMLENGTH);		
		String mediaStreamDirResourceNID = MRXMLUtils.getAttributeValue(requestEl, SessionServiceResourceConst.MEDIA_STREAM_DIR_RESOURCE_NID_ATTRIBUTE, "");
		
		if (enable.equalsIgnoreCase(SessionServiceConst.FALSE)) {
			trimLength = ""; mediaStreamDirResourceNID = "";
		} else if (enable.equalsIgnoreCase(SessionServiceConst.TRUE) && mediaStreamDirResourceNID.isEmpty()) {
			throw new MRException (SessionServiceExceptionCodes.PVR_MEDIA_DIR_NOT_FOUND, "Media directory to store PVR files is missing");
		}
		
		// Get port objectXML doc 
		Document objectXMLDoc = this.getPortObjectXMLDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		String portType = MRXMLUtils.getValueXpath(objectXMLDoc, "//" + SessionServiceResourceConst.INFO_ELEMENT 
				+ "/" + SessionServiceConst.TYPE_EL_NAME, "");		
		if (!portType.equals(SessionServiceConst.STREAM_SRC) && (enable.equalsIgnoreCase(SessionServiceConst.TRUE))) {
			throw new MRException(SessionServiceExceptionCodes.INVALID_PORT_TYPE, "This operation is allowed only on a source port");
		}
		
		String parentElXPath = "//" + SessionServiceResourceConst.PVR_INFO_ELEMENT;
		
		// Set & update
		MRXMLUtils.setAttributeValueXPath(objectXMLDoc, parentElXPath, SessionServiceResourceConst.ENABLE_ATTRIBUTE, enable);		
		MRXMLUtils.setAttributeValueXPath(objectXMLDoc, parentElXPath, SessionServiceConst.TRIMLENGTH, trimLength);
		MRXMLUtils.setAttributeValueXPath(objectXMLDoc, parentElXPath, SessionServiceResourceConst.MEDIA_STREAM_DIR_RESOURCE_NID_ATTRIBUTE, mediaStreamDirResourceNID);
		
		//Validate the request to verify if update is allowed, later we will update the db in PVRManager once the PVRSession is ready/stopped.
		Element rootEl = objectXMLDoc.getRootElement();
		String NID = MRXMLUtils.getAttributeValue(rootEl, SessionServiceConst.NID, "");
		UpdateObjectUtils.validateElementBeforeUpdate(request, xmpp, log, db, rootEl.getName(), NID, objectXMLDoc,  parentElXPath);
		
		if (enable.equalsIgnoreCase(SessionServiceConst.TRUE)) {										
			// restartSession='true' allows updating an existing session. Useful in case of SetPortPVRInfo updates
			AppServerAgentUtils.getPVRManager(serviceAgentFactory).startPersistentPVRSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, objectXMLDoc, true);				
		} else {
			AppServerAgentUtils.getPVRManager(serviceAgentFactory).stopPersistentPVRSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, objectXMLDoc);
		}
		
		return updatePort(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, objectXMLDoc, parentElXPath);
		
	}
		
}

