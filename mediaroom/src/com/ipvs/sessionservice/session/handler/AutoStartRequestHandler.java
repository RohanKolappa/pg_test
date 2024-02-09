package com.ipvs.sessionservice.session.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.ConnectionUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.sessionservice.handler.base.AsyncBaseRequestHandler;
import com.ipvs.sessionservice.impl.Session;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;

public class AutoStartRequestHandler extends AsyncBaseRequestHandler {	
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
						
		Document requestDoc = request.getMessage().getDocument();	
		String portNID = MRXMLUtils.getAttributeValueXpath(requestDoc, "//AutoStart","portNID" , "");
		MRRequest newRequest=MRRequest.createRequest(request.getAppServerJID(), request.getAppServerJID());	
		log.addInfo("AutoStartRequestHandler:getResponse:: PortNID: "+portNID, MRLog.OK, MRLog.NOTIFY);
		Document mediaSrcPortDoc = MRXMLDBUtils.readElement(request, log, db,
				portNID);
		String toAgentJID = MediaRoomServiceUtils.getAgentJID(request, mediaSrcPortDoc); //if empty or state offline return error
//		String state = MRXMLUtils.getValueXpath(mediaSrcPortDoc, "//"+AMConst.STATEFLAG, "");
//		if(!state.equals(MRClientUtils.ASSET_STATE_READY) || toAgentJID.isEmpty()) {
//			return null;
//		}
		String portTitle = MRXMLUtils.getValueXpath(mediaSrcPortDoc,"//Info/Title","");		
		Element portElement=MRXMLUtils.stringToElement("<SrcPort id='' portNID='"+portNID+"' portTitle='"+portTitle+"' pvrEnable='' trimLength=''/>");		
		String streamType = MRXMLUtils.getValueXpath(mediaSrcPortDoc, "//StreamTypeList/StreamType", "");
		log.addInfo("AutoStartRequestHandler:getResponse:: streamType: "+streamType, MRLog.OK, MRLog.NOTIFY);		
		Element srcProfileEle = MRXMLUtils.getElementXpath(mediaSrcPortDoc, "//MediaStreamProfileInfo/"+streamType+"MediaStreamProfileInfo");
		Element infoEle = (Element)srcProfileEle.clone();
		infoEle.setName(streamType+SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO);
		Element profileEle = MRXMLUtils.stringToElement("<profileXML/>");
		profileEle.add(infoEle);		
		String sessionId = startSession(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, profileEle, portElement, toAgentJID,portNID, portTitle, streamType);
		MRXMLUtils.setAttributeValueXPath(mediaSrcPortDoc, "//Instance", "sessionId", sessionId);
		Element portEl = mediaSrcPortDoc.getRootElement();
		boolean isSuperUser = request.isSuperUser();
		if(!isSuperUser)
			request.setSuperUser(true);
		UpdateObjectUtils.updateElement(request, xmpp, log, db, portEl.getName(), portNID, mediaSrcPortDoc, "//Instance");
		request.setSuperUser(isSuperUser);
		Element responseData = MRXMLUtils.stringToElement("<AutoStart portNID='"+portNID+"' sessionId='"+sessionId+"'/>");
		return request.createResponseData(responseData);		
	
	}

	public void handleAgentResponse(MRResponseHandlerI responseHandler,
		MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
		MRResourceFactoryI resourceFactory,
		ServiceAgentFactoryI serviceAgentFactory) throws Exception {

	    MRRequest newRequest = MRRequest.createRequest(request.getUserJID());
	    Document responseDoc = request.getResult();		
	    String streamURL = MRXMLUtils.getValueXpath(responseDoc, "//"+SessionServiceConst.URL, "");		
	    String streamNID = MRXMLUtils.getAttributeValueXpath(responseDoc, "//StreamStatus",SessionServiceConst.STREAM_NID , "");		
	    String roomNID = MRXMLUtils.getAttributeValueXpath(responseDoc, "//StreamStatus",SessionServiceConst.ROOMNID , "");
	    log.addInfo("AutoStartRequestHandler:handleAgentResponse::roomNID= "+roomNID+ ": StreamURL"+streamURL, MRLog.OK, MRLog.NOTIFY);
	    Document streamDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, newRequest, db, log, streamNID));
	    try{
	    	String sourceNID = MRXMLUtils.getValueXpath(streamDataDoc, ".//"+SessionServiceConst.SOURCE_NID , ""); 
	    	Document mediaSrcPortDoc = MRXMLDBUtils.readElement(newRequest, log, db,
	    			sourceNID);	

	    	Element mediaStreamURLEl = MRXMLUtils.stringToElement("<MediaStreamURL>"+streamURL+"</MediaStreamURL>");

	    	//only update the port info when the device is configured for autostart, this way the request handler can be used in other modes as well
	    	if(MRXMLUtils.getAttributeValueXpath(mediaSrcPortDoc, "//Instance", "mode", SessionServiceConst.MODE_DYNAMIC).equalsIgnoreCase(SessionServiceConst.MODE_AUTO_START))

	    	{
	    	    /*if(streamURL.isEmpty() || streamURL.equals("")){
	    		MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
				String.valueOf(MediaRoomServiceConst.START_ERROR));
	    	    }
	    	    else{*/
	    	
	    		boolean isSuperUser = request.isSuperUser();
	    		if(!isSuperUser)
	    			request.setSuperUser(true);
	    		MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, "", mediaSrcPortDoc.getRootElement().getName(), sourceNID, "//"+SessionServiceConst.MEDIA_STREAM_URL, null, mediaStreamURLEl);
	    		MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
	    				String.valueOf(MediaRoomServiceConst.STREAM_STARTED));
	    		request.setSuperUser(isSuperUser);
	    	    //}
	    	}
	    }
	    catch(Exception e){
		MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
			String.valueOf(MediaRoomServiceConst.START_ERROR));
	    }

	}
	protected String startSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element profileXML, Element portElement, String toAgentJID, String portNID, String portTitle, String streamType) throws Exception {
		
		String sessionId = portNID + "." + SessionServiceConst.MODE_AUTO_START + SessionServiceConst.SESSION;
		String sessionName = portTitle + "." + SessionServiceConst.MODE_AUTO_START + SessionServiceConst.SESSION;
		//Semaphore lock = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());
		String returnSessionId = sessionId;
        log.addInfo("AutoStartRequestHandler:startSession aquired lock for SessionID= "+sessionId, MRLog.OK, MRLog.NOTIFY);
	
        /*
         * create session document
         */
        try{
            Document sessionTemplateDoc = Session.getSessionTemplate(log);			
            Element sessionTemplateRootEl = sessionTemplateDoc.getRootElement();
            sessionTemplateRootEl.addAttribute(SessionServiceConst.NAME, sessionName);
            sessionTemplateRootEl.addAttribute(SessionServiceConst.GC, MRConst.TRUE);		
            sessionTemplateRootEl.addAttribute(SessionServiceConst.ID, sessionId);

            Document sessionDoc = MRXMLUtils.elementToDocument(sessionTemplateRootEl);
            XMPPTransaction.startBlock("AutoStartRequestHandler");

            SessionResourceUtils.updateSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, sessionId, "");
            XMPPTransaction.startBlock("addSourcesAndConnections");
            // Create the Src Entities and Connections
            this.addSourcesAndConnections(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc,portElement,streamType);
            String srcEntityId = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//"+SessionServiceConst.ENTITY, SessionServiceConst.ID, "");
            XMPPTransaction.endBlock("addSourcesAndConnections");

            //Set Auto Start Session Type here
            MRXMLUtils.setAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.TYPE_SYSTEM_READ_ONLY);

            //SessionPolicyUtils.enforcePolicies(request, xmpp, log, db, resourceFactory, serviceAgentFactory, startSessionRequestDoc);
            XMPPTransaction.startBlock("CreateSession");
            String roomNID = SessionMediaRoomUtils.createSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
            XMPPTransaction.endBlock("CreateSession");		
            /*
             * add media source
             */
            try{

        	String	mediaSourceNID = SessionMediaRoomUtils.createMediaSource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcEntityId, sessionDoc, roomNID);
        	Document roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);		
        	Element mediaSourceDataDoc = MRXMLUtils.getElementXpath(roomdoc, "//MediaSource[@id='" + srcEntityId + "']");

        	/*
        	 * add media stream
        	 */
        	String description = "ConnectionDescription";
        	String id =MRXMLUtils.getAttributeValueXpath(sessionDoc,  "//"+SessionServiceConst.CONNECTION, SessionServiceConst.ID, "");;
        	Document mediaStreamResponse = ConnectionUtils.addMediaStream(request,xmpp, log, db, resourceFactory, serviceAgentFactory,"AutoStartStreamRequest", 
        		id, roomNID, mediaSourceNID, "", description, "", profileXML, "",id,sessionId);

        	String streamNID = MRXMLUtils.getValueXpath(mediaStreamResponse,"//streamNID", "");

        	/*
        	 * start connection	
        	 */			

        	Document streamDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID));		
        	sendStartRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID, streamNID, streamDataDoc, MRXMLUtils.elementToDocument(mediaSourceDataDoc), toAgentJID);
        	
            }catch(Exception e){
        	SessionMediaRoomUtils.stopViewSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, sessionId);
        	//throw new MRException(MRExceptionCodes.DB_NOT_INITIALIZED,"Device is not ready");
        	returnSessionId= "";
            }
        }catch(Exception ex){
            returnSessionId= "";
        }
        finally{
           // IPVSMonitor.getMonitor().releaseLock(sessionId, lock);
           
        }
        return returnSessionId;
	}
	
	public void sendStartRequest(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory,String roomNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc,String toAgentJID) throws MRException,Exception {
		
	    String actionNID = roomNID;
	    Element actiondata = MRXMLUtils.stringToElement("<actiondata/>");
	    String actionMessage = StreamAgentUtils.createSourceStreamAgentActionRequestMessage(streamNID, MediaRoomServiceConst.START, MediaRoomServiceConst.STARTREQUEST, 
		    actiondata, 0, actionNID, streamDataDoc, mediaSourceDataDoc, log);	     

	    AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
		    toAgentJID, "StartRequest", MRConst.MEDIA_SERVICE, actionMessage, this.getClass().getName());

	}

	protected Document addSourcesAndConnections(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Document startSessionRequestDoc, Element portElement, String streamType) throws Exception {		
		String resourceType =SessionServiceConst.SRC_PORT;
		Element entity= UpdateSessionUtils.createPortEntity(request, xmpp, log,db, resourceFactory,serviceAgentFactory, portElement , resourceType);
		entity.addAttribute(SessionServiceConst.STREAM_TYPE, streamType);
		String srcEntityId = entity.attributeValue(SessionServiceConst.ID);
		log.addInfo("AutoStartStreamRequestHandle: addSourcesAndConnections:srcEntityId: "+srcEntityId, MRLog.OK, MRLog.NOTIFY);
		Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add(entity);
		String connectionId = Utils.getUUID();
		Element connection = UpdateSessionUtils.createConnection(connectionId, "", srcEntityId, "", startSessionRequestDoc, "", "", "");		
		Element connectionList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.CONNECTIONLIST);		
		connectionList.add(connection);
		return startSessionRequestDoc;
	}
	/*
	 * Test method: this method will go away once the profile xml is populated in the port xml
	 */
	public Element getDummyProfileXml(){
		Element profileEle = null;
		try {
			profileEle = MRXMLUtils.stringToElement("<profileXML/>");
			Element infoEle = MRXMLUtils.stringToElement("<Info> <V2DStreamConfig> <IsMulticast>false</IsMulticast>  <EnableSRDTranscode>true</EnableSRDTranscode> <SRDTranscodeBandwidth></SRDTranscodeBandwidth> <AVOption>Both</AVOption>" +
					" </V2DStreamConfig> <V2DEncoderAudioConfig> <SampleRate></SampleRate><SampleSize></SampleSize><MonoStereo></MonoStereo><AudioEncodeEnable></AudioEncodeEnable> </V2DEncoderAudioConfig><V2DEncoderConnectionParameterConfig>" +
					"<BurstRatio>1</BurstRatio> <BurstDuration>50</BurstDuration> <Compression> <High>6</High> <Low>6</Low> </Compression>  <Bandwidth>10000</Bandwidth> <Refresh> <Min>10</Min> <Max>30</Max></Refresh><Slice><Min>0</Min><Max>15</Max>" +
					" </Slice> <BlockThreshold>5</BlockThreshold><ColorSampling>4:2:2</ColorSampling><FrameRateDivider>1</FrameRateDivider><IFrameInterval></IFrameInterval></V2DEncoderConnectionParameterConfig><V2DEncoderConnectionParameterKbmConfig>" +
					" <AllowKMSwitch>true</AllowKMSwitch> <KMIdleTime>0</KMIdleTime><EnableKbm>false</EnableKbm></V2DEncoderConnectionParameterKbmConfig> <V2DRecorderConfig> <FFTracks>1-256</FFTracks> " +
					"<ThumbnailConfig> <Enable>true</Enable></ThumbnailConfig> </V2DRecorderConfig></Info>");
			profileEle.add(infoEle);
		} catch (Exception e) {			
			e.printStackTrace();
		}
				
		return profileEle;		
	}	
}

