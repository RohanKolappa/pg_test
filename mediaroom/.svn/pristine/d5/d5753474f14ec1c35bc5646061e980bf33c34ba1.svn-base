package com.ipvs.assetadminservice.port.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.AutoStartManager.Action;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


public class SetPortResourceTypeRequestHandler extends SetPortBaseRequestHandler {
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);
		return auth;
	}


	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		log.addInfo("BaseServiceClientRequestHandler: Debug log added by padmaja in port type update ", MRLog.INFO, MRLog.NOTIFY);
		
		Element requestElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//"+MRRequest.getRequestRootName(request.getRequestName()));
		String byAssetNID = MRXMLUtils.getAttributeValue(requestElement,AMConst.BY_ASSET_NID_ATTRIBUTE);
		String[] xpathArr = { "//AssetResourceNID[.='" + byAssetNID + "']" };
		Document responseDoc = MRXMLUtils.getDocument();
		responseDoc.add(MRXMLUtils.newElement("ResponseDoc"));   	 
		MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT,request,log,db, "", AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE, "","",xpathArr, null,"", "", 
				responseDoc.getRootElement(), 0,-1);

		List<?> listOfPorts = MRXMLUtils.getList(responseDoc.getRootElement());
		if(listOfPorts == null || listOfPorts.isEmpty()){
			throw new MRException(AMExceptionCodes.NO_PORT_FOUND_IN_DATABASE,"No Ports Found in Database");
		}

		List<?> listOfRequestedPorts = MRXMLUtils.getList(requestElement);
		for(Object object  :listOfRequestedPorts) {
			Element requestedPortResourceEl = (Element) object;
			String portId = MRXMLUtils.getAttributeValue(requestedPortResourceEl, AMConst.DEVICE_ELEMENT_ID_ATTRIBUTE,"");
			String xpath  = "//MediaStreamIOPortResource[.//DeviceElementID='"+portId+"']";
			Element portResourceEl = MRXMLUtils.getElementXpath(responseDoc, xpath);
			String persistentPVRAction = "";
			if(portResourceEl != null)  {
			    	String portNID = MRXMLUtils.getAttributeValue(portResourceEl, AMConst.NID,"");
				String portType = MRXMLUtils.getValueXpath(portResourceEl, ".//Info/Type", "");
				String newPortType = MRXMLUtils.getAttributeValue(requestedPortResourceEl, AMConst.TYPE_ATTRIBUTE);
				String thumbnailSupport = MRXMLUtils.getAttributeValue(requestedPortResourceEl, "thumbnail");

				
				if(!portType.equals(newPortType)) {
					MRXMLUtils.setValueXpath(portResourceEl, ".//Info/Type", newPortType);
					
					Element featureListEL = MRXMLUtils.getElementXpath(portResourceEl, ".//FeatureList");
					if(featureListEL!= null) {
						MRXMLUtils.removeContent(featureListEL);
					}
					Action action = null;
					String mode= MRXMLUtils.getAttributeValueXpath(portResourceEl, ".//Instance", "mode", SessionServiceConst.MODE_DYNAMIC);
					if(SessionServiceConst.MODE_AUTO_START.equals(mode)){
					     action = Action.CONTINUE_AUTOSTART_SESSION;
					}
					//update feature information based on the new type				
					if(newPortType.equals(SessionServiceConst.STREAM_DST)){
						
						//add Display config feature
						Element featureEL = MRXMLUtils.addElement("Feature");
						MRXMLUtils.setAttributeValue(featureEL, "Name", SessionServiceConst.XP200DISPLAYCONFIG);
						MRXMLUtils.addElement(featureListEL, featureEL);
						
						
						//add trick play feature 
						featureEL = MRXMLUtils.addElement("Feature");   
						MRXMLUtils.setAttributeValue(featureEL, "Name", SessionServiceResourceConst.TRICK_PLAY_FEATURE_NAME);
						MRXMLUtils.setValue(featureEL, SessionServiceConst.TRUE);
						MRXMLUtils.addElement(featureListEL, featureEL);
						//stop auto start session if present						
						if(SessionServiceConst.MODE_AUTO_START.equals(mode)){

						    Element streamTypeEle = MRXMLUtils.getElementXpath(portResourceEl, ".//StreamTypeList[StreamType='"+MediaRoomServiceConst.V2D+"']");
						    //currently only auto starting the device with v2d streams
						    if(streamTypeEle!= null){
							action = Action.STOP_AUTOSTART_SESSION;							
							log.addInfo("SetPortResourceTypeRequestHandler:: Changed to dest: Stopping autostart session: portNID="+portNID, MRLog.OK, MRLog.NOTIFY);
							
						    }
						}
						//handle persistent pvr
						String persistentPVRMode = MRXMLUtils.getAttributeValueXpath(portResourceEl, ".//PVRInfo", "enable", SessionServiceConst.FALSE);
						if(SessionServiceConst.TRUE.equals(persistentPVRMode)){
						 
						    persistentPVRAction = SessionServiceConst.STOP_PVR_SESSION;
						}
					} 
					if(newPortType.equals(SessionServiceConst.STREAM_SRC)){
					        //add Thumbnail feature
					       if(thumbnailSupport.equals("true")){
			                Element featureEL = MRXMLUtils.addElement("Feature");       
			                MRXMLUtils.setAttributeValue(featureEL, "Name", SessionServiceConst.THUMBNAIL);
			                MRXMLUtils.addElement(featureListEL, featureEL);
					         }
					    Element streamTypeEle = MRXMLUtils.getElementXpath(portResourceEl, ".//StreamTypeList[StreamType='"+MediaRoomServiceConst.V2D
					    		+ "' or StreamType='"+MediaRoomServiceConst.RTP_RAW
					    		+ "' or StreamType='"+MediaRoomServiceConst.RTP_PCM+"']");
					    if(streamTypeEle!= null){			
					    	
					    	mode= MRXMLUtils.getAttributeValueXpath(portResourceEl, ".//Instance", "mode", SessionServiceConst.MODE_DYNAMIC);							
					     	//restart only if auto start mode
					     	if(mode.equals(SessionServiceConst.MODE_AUTO_START)){					     								
					     		action = Action.RESTART_AUTOSTART_SESSION;
					     		log.addInfo("SetPortResourceTypeRequestHandler:: Changed to source: Set to autostart : portNID="+portNID, MRLog.OK, MRLog.NOTIFY);
					     	}
					    }
					    String persistentPVRMode = MRXMLUtils.getAttributeValueXpath(portResourceEl, ".//PVRInfo", "enable", SessionServiceConst.FALSE);
						if(SessionServiceConst.TRUE.equals(persistentPVRMode)){
						    persistentPVRAction = SessionServiceConst.RESTART_PVR_SESSION;
						    
						}
					}				
					
					MRXMLDBUtils.updateElement(request, log, db, portNID, portResourceEl);
					if (request.getCache() == null ) {
                        request.setCache(new MRRequestCache());
                    }
					request.getCache().put(portNID,MRXMLUtils.elementToDocument(portResourceEl));
					//generate port notification
					Element portElement =  MRXMLUtils.addElement("SetPort");
					portElement.addAttribute(SessionServiceConst.WHERE_NID, portNID);
					MRRequest newRequest  = MRRequest.createRequest(request,MRXMLUtils.elementToDocument(portElement),"SetPortRequest","","");
					RequestProcessor requestObject = new RequestProcessor(newRequest, xmpp, log, db, resourceFactory, profileFactory, SessionServiceConst.PORT);
					super.generatePortUpdateNotifications(request, xmpp, log, db, resourceFactory, profileFactory, requestObject, SessionServiceConst.PORTTYPECHANGE);
					
					request.setSuperUser(true);
					if(action != null) {
                        AppServerAgentUtils.getAutoStartManager(profileFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, profileFactory, portNID, action,"");
                    }
					log.addInfo("SetPortResourceTypeRequestHandler:Update Port Type. NID:="+ MRXMLUtils.getAttributeValue(portResourceEl, AMConst.NID,"")
							+" Title:="+MRXMLUtils.getValueXpath(portResourceEl, ".//Info/Title", "") 
							+" OldPortType:="+portType +" NewPortType:="+newPortType +" portId:="+portId,MRLog.INFO,MRLog.NOTIFY);
					if(SessionServiceConst.RESTART_PVR_SESSION.equals(persistentPVRAction)) {
                        AppServerAgentUtils.getPVRManager(profileFactory).startPersistentPVRSession(request, xmpp, log, db, resourceFactory, profileFactory, MRXMLUtils.elementToDocument(portResourceEl),true);
                    } else if(SessionServiceConst.STOP_PVR_SESSION.equals(persistentPVRAction)) {
                        AppServerAgentUtils.getPVRManager(profileFactory).stopPersistentPVRSession(request, xmpp, log, db, resourceFactory, profileFactory, MRXMLUtils.elementToDocument(portResourceEl));
                    }
					
				}
			}
		}

		return request.createResponseData();
	}

}
