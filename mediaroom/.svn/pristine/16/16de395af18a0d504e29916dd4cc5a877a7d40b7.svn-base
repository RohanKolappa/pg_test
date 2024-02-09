package com.ipvs.sessionservice.resource.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.AutoStartManager.Action;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;



public class SetPortAutoStartRequestHandler extends SetPortBaseRequestHandler{

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String parentElXPath = "//Instance";
		request.setCache(new MRRequestCache());
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.PORT);
		
		//by title is not working - fixed by Padmaja
		Document srcPortDoc =  getPortObjectXMLDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		String portNID =MRXMLUtils.getAttributeValue(srcPortDoc.getRootElement(), SessionServiceConst.NID,"");
		
		//Don't allow updating auto start config if tx is part of some streams.
		String whereXpath = "//MediaRoom[.//Stream/data[sourceNID='"+portNID+"' and destNID!='']]";
		Document roomListDoc = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, whereXpath, false);
		List<?> roomList = MRXMLUtils.getListXpath(roomListDoc, "//MediaRoom");
			
		if(roomList!=null && roomList.size()>0) {
			throw new MRException(SessionServiceExceptionCodes.CAN_NOT_UPDATE_AUTO_START_SESSION_IN_USE, "Can not update Auto Start Config , Source is in use.");
		}
		
		String enableAutoStart = MRXMLUtils.getAttributeValue(requestObject.getRequestElement(),"enable" , "");
		
		// Update the cache
		request.getCache().put(portNID, srcPortDoc);
		
		Document responseDoc;
		if(enableAutoStart.equalsIgnoreCase("true")){

			String streamType = MRXMLUtils.getValueXpath(srcPortDoc, "//StreamTypeList/StreamType", "");

			Element profileXmlEl = MRXMLUtils.getElementXpath(requestObject.getRequestElement(), ".//ProfileXML");
			if(profileXmlEl.hasContent()){
				Element srcPortProfileEl = (Element)profileXmlEl.clone();
				//MRXMLUtils.setValueXpath(srcPortProfileEl, ".//"+streamType+SessionServiceConst.STREAMCONFIG+"/"+SessionServiceConst.ISMULTICAST, SessionServiceConst.TRUE);
				String profileName = streamType+SessionServiceConst.MEDIASTREAMPROFILEINFO;
				srcPortProfileEl.setName(profileName);				
				
				Element mediaStreamProfileInfo = MRXMLUtils.stringToElement("<"+SessionServiceConst.MEDIASTREAMPROFILEINFO+"/>");
				mediaStreamProfileInfo.add(srcPortProfileEl);
				
				MRXMLUtils.replaceContent(srcPortDoc, "//"+SessionServiceConst.MEDIASTREAMPROFILEINFO, mediaStreamProfileInfo);
	
				MRXMLUtils.setAttributeValueXPath(srcPortDoc, "//Instance", SessionServiceConst.MODE, SessionServiceConst.MODE_AUTO_START);
				//thumbnail were getting stopped, as the enable value was empty, fixed by Neha
				MRXMLUtils.setValueXpath(srcPortDoc, "//ThumbnailConfig/Enable", MediaRoomServiceConst.TRUE);
				responseDoc = updatePort(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, srcPortDoc, parentElXPath);
				AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.RESTART_AUTOSTART_SESSION,"");
			}
			else{
				throw new MRException(MRExceptionCodes.PROFILE_EMPTY, "Profile Information not Found");
			}
		}
		else{
		    	//AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.STOP_AUTOSTART_SESSION);
		    	AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).stopPersistentAutoStartSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcPortDoc);
		    	//srcPortDoc = MRXMLDBUtils.readElement(request, log, db,	portNID);
			MRXMLUtils.setAttributeValueXPath(srcPortDoc, "//Instance", SessionServiceConst.MODE, SessionServiceConst.MODE_DYNAMIC);
			responseDoc = updatePort(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, srcPortDoc, parentElXPath);
		}
		 
		return responseDoc;
	}
}
