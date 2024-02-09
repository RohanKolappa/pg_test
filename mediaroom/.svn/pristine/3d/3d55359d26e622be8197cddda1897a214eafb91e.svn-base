package com.ipvs.mediaroomservice.handler;

import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;


public class AddStreamActionRequestHandler implements MRRequestHandlerI , AsynchDBRequestResponseListenerI {

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
	throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.STREAM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;

		// If destNID is empty then use the request.getResourceGroup
		// as the destGroup to find the matching policy
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);

		return auth;
	}

    private void updateStreamDataWithTrackInfo(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		String streamNID, Document streamDataDoc, Document mediaSourceDataDoc) throws Exception {
    	
    	XMPPTransaction.startBlock("updateStreamDataWithTrackInfo");

    	String roomNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//roomNID", "");
    	String trackNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID", "");
	    // Update the trackState and trackOffset in the streamDataDoc
		Document trackDataDoc1 = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
        MediaTimeline trackTimeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc1, log);

		String trackState = trackTimeline.getLastState();
		
		long trackOffset = trackTimeline.getLastOffset();
		
		long lastActionWallClock = trackTimeline.getLastActionWallClock();

		long now = xmpp.getCurrentTimeMillis();
		if (MediaRoomServiceConst.TRACK_STARTED_CURRENT.equals(trackState) || 
				MediaRoomServiceConst.TRACK_STARTED_LOOKBACK.equals(trackState)) {
	        trackOffset = trackOffset + (now - lastActionWallClock);
	    }
		MRXMLUtils.getElementXpath(streamDataDoc, "//startWallClock").setText(String.valueOf(now));
		MRXMLUtils.getElementXpath(streamDataDoc, "//trackStateAtStart").setText(trackState);
		MRXMLUtils.getElementXpath(streamDataDoc, "//trackOffsetAtStart").setText(String.valueOf(trackOffset));
        MediaRoomServiceUtils.setObjectData(roomNID, request, db, log, streamNID, MRXMLUtils.documentToString(streamDataDoc));
    	XMPPTransaction.endBlock("updateStreamDataWithTrackInfo");
    }

	public void sendSetupRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI serviceAgentFactory, String roomNID, String streamNID, String destAgentJID, Document streamDataDoc, Document mediaSourceDataDoc) throws Exception {
	    String streamdatadocStr = URLEncoder.encode(MRXMLUtils.documentToString(streamDataDoc), "UTF-8");
	    String mediasourcedatadocStr = URLEncoder.encode(MRXMLUtils.documentToString(mediaSourceDataDoc), "UTF-8");
        String serviceName = MRConst.LAYOUT_SERVICE;
	    ServiceAgentUtils.sendAgentRequestMessage( xmpp, log, roomNID,AppServerAgent.class.getName(), destAgentJID, serviceName, MediaRoomServiceConst.SETUPREQUEST,
	            "<data>" + 
	            "<streamNID>" + streamNID + "</streamNID>" +
	    		"<streamdatadoc>" + streamdatadocStr + "</streamdatadoc>" +
	    		"<mediasourcedatadoc>" + mediasourcedatadocStr + "</mediasourcedatadoc>" +
	            "</data>",
	            "AddStreamRequestHandler:=" + request.getUserJID() + "/" + streamNID + " > Setup Message to " +
	            ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.DESTSTREAMAGENT), request.getNID(),request.getAppServerJID(),MRConst.LAYOUT_SERVICE,"");
	}
	
	public void sendTeardownRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI serviceAgentFactory, String roomNID, String streamNID, String destAgentJID) throws Exception {
        String serviceName = MRConst.LAYOUT_SERVICE;
	    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, roomNID,AppServerAgent.class.getName(), destAgentJID, serviceName, MediaRoomServiceConst.TEARDOWNREQUEST,
	            "<data>" + 
	            "<streamNID>" + streamNID + "</streamNID>" +
	            "</data>",
	            "AddStreamRequestHandler:=" + request.getUserJID() + "/" + streamNID + " > Teardown Message to " +
	            ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.DESTSTREAMAGENT), request.getNID(),request.getAppServerJID(),MRConst.LAYOUT_SERVICE,"");
	}

    private String addStreamAction(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory, String streamNID, Element data, String updateAction, Document streamDataDoc)
        throws Exception {
    	
    	XMPPTransaction.startBlock("addStreamAction");
        String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
        String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
        String sourceAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID","");
        String actionNID = String.valueOf(xmpp.getCurrentTimeMillis());
        log.addInfo("AddStreamActionRequestHandler: Added actionNID=" + actionNID);
  
        // MediaRoomServiceUtils.updateMediaDestinationStatus(request, db, log, streamNID, roomNID, MediaRoomServiceConst.STREAM_DST_PRESENCE_XPATH, MediaRoomServiceConst.AVAILABLE);
        if(MediaRoomServiceConst.START.equals(updateAction)) {
            if("".equals(destAgentJID) || "".equals(sourceAgentJID)) { // Waiting for source/dest contact JID
                MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
                		String.valueOf(MediaRoomServiceConst.WAITING_FOR_CONTACTJID));
                log.addInfo("AddStreamActionRequestHandler:addStreamAction waiting for contactJID sourceAgentJID=" + 
                		sourceAgentJID + " destAgentJID=" + destAgentJID, MRLog.OK, MRLog.NOTIFY);
                return actionNID;
            }
            

            if (RoomDB.getRoomDB().isReady(xmpp, roomNID, destAgentJID,  request.getUserJID(), request.getAppServerJID())) {
            	XMPPTransaction.startBlock("sendWatch");
            	XMPPTransaction.startBlock("getMediaSource");
                String mediaSourceNID = MRXMLUtils.getElementXpath(streamDataDoc, "//mediaSourceNID").getStringValue();
                String mediasourcedatadocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID);
            	Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(mediasourcedatadocStr);
            	XMPPTransaction.endBlock("getMediaSource");
            	this.updateStreamDataWithTrackInfo(request, xmpp, log, db, streamNID, streamDataDoc, mediaSourceDataDoc);
            	// XMPPTransaction.startBlock("encode");
                // String streamdatadocStr = URLEncoder.encode(MRXMLUtils.documentToString(streamDataDoc), "UTF-8");
                // mediasourcedatadocStr = URLEncoder.encode(mediasourcedatadocStr, "UTF-8");
            	// XMPPTransaction.endBlock("encode");
            	XMPPTransaction.startBlock("sendAgentRequest");
            	
            	//if destination is Appserver (managementserver.localhost)  then send start request 
            	if(destAgentJID.equals(AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(), serviceAgentFactory))) {
            		new HandleUnmanagedDestinationStreams().sendStartRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID, streamNID, streamDataDoc, mediaSourceDataDoc);            	
            	} else {
	            	this.sendSetupRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
	            			roomNID, streamNID, destAgentJID, streamDataDoc, mediaSourceDataDoc);            	
	            	
	            	XMPPTransaction.endBlock("sendAgentRequest");
	            	XMPPTransaction.startBlock("updateStreamStatus");
	                MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
	                		String.valueOf(MediaRoomServiceConst.WATCH_SENTTO_DEST));
	            	XMPPTransaction.endBlock("updateStreamStatus");
		        	XMPPTransaction.endBlock("sendWatch");
            	}
            }
            else
            	MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
            		String.valueOf(MediaRoomServiceConst.WAITING_FOR_DEST));
        }
        else if(MediaRoomServiceConst.STOP.equals(updateAction)) {
            MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
            		String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
    		MRXMLUtils.getElementXpath(streamDataDoc, "//startWallClock").setText(String.valueOf(0));        	
    		MRXMLUtils.getElementXpath(streamDataDoc, "//trackStateAtStart").setText("");
    		MRXMLUtils.getElementXpath(streamDataDoc, "//trackOffsetAtStart").setText("");
            MediaRoomServiceUtils.setObjectData(roomNID, request, db, log, streamNID, MRXMLUtils.documentToString(streamDataDoc));
            if("".equals(destAgentJID) || "".equals(sourceAgentJID)) { // Waiting for source/dest contact JID
                return actionNID;
            }
            if (RoomDB.getRoomDB().isReady(xmpp, roomNID, destAgentJID, request.getUserJID(),  request.getAppServerJID())) {
            	
            	//if destination is Appserver (managementserver.localhost)  then send start request 
            	if(destAgentJID.equals(AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(), serviceAgentFactory))) {
            	    String mediaSourceNID = MRXMLUtils.getElementXpath(streamDataDoc, "//mediaSourceNID").getStringValue();
                    String mediasourcedatadocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID);
                	Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(mediasourcedatadocStr);
            		new HandleUnmanagedDestinationStreams().sendStopRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID, streamNID, streamDataDoc, mediaSourceDataDoc);
            		MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
                    		String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
            	} else {    
            		this.sendTeardownRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
            			roomNID, streamNID, destAgentJID);
            	}
            }
        }
    	XMPPTransaction.endBlock("addStreamAction");
        return actionNID;
    }

    public Document addStreamAction(Element dataEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	/*Element data = MediaRoomServiceUtils.cloneDataElement(request);*/
    	com.ipvs.common.XMPPTransaction.startBlock("AddStreamActionRequest");
        String streamNID = MRXMLUtils.getValue(dataEL, "streamNID","");
        String updateAction = MRXMLUtils.getValue(dataEL,"updateAction","");
        
        String roomNID = MRXMLUtils.getValueXpath(dataEL, ".//roomNID", "");
  	  	if("".equals(roomNID))
  	  		roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);
		
    	
  	  	com.ipvs.common.XMPPTransaction.startBlock("GetStreamDataDoc");
        String streamDataDocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID);
  	  	com.ipvs.common.XMPPTransaction.endBlock("GetStreamDataDoc");
        if (streamDataDocStr == null) {
            throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND,MRXMLUtils.generateErrorXML("streamNID", streamNID));
        }
    	Document streamDataDoc = MRXMLUtils.stringToDocument(streamDataDocStr);

        String actionNID = "";
        String streamAction = updateAction;
        
        
        if(MediaRoomServiceConst.AUTHORIZE.equals(updateAction)) {        	
        	actionNID = handleAuthorize(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID, streamNID, dataEL, streamDataDoc);
        } else if(MediaRoomServiceConst.DELETE.equals(updateAction)) {
        	streamAction = MediaRoomServiceConst.STOP;
        }
        
                
		if (MediaRoomServiceConst.START.equals(streamAction) || MediaRoomServiceConst.STOP.equals(streamAction)) {        	
			//perf-logs  log.addInfo("AddStreamActionRequestHandler::=" + request.getUserJID()) + " addStreamAction - Start", MRLog.OK, MRLog.NOTIFY);
            actionNID = this.addStreamAction(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamNID, dataEL, streamAction, streamDataDoc);
            //perf-logs  log.addInfo("AddStreamActionRequestHandler::=" + request.getUserJID()) + " addStreamAction - End", MRLog.OK, MRLog.NOTIFY);
        } else if(!MediaRoomServiceConst.AUTHORIZE.equals(streamAction)) {
            throw new MRException(MediaRoomServiceExceptionCodes.ACTIONNOTFOUND, MRXMLUtils.generateErrorXML("updateAction", updateAction));
        }
		
		
	    if(MediaRoomServiceConst.DELETE.equals(updateAction)) {
        	Document streamDoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
        	MediaRoomServiceUtils.deleteStream(xmpp, request, log, db, roomNID, streamNID);       
        	
        	//release multicast ipaddress
            String sourceNID = MRXMLUtils.getValueXpath(streamDoc, "//sourceNID", "");
        	String relayNID = MRXMLUtils.getValueXpath(streamDoc, "//relayNID", "");
        	String multicastIP = MRXMLUtils.getValueXpath(streamDoc, "//multicastIP", "");
        	AppServerAgentUtils.getServiceConfigManager(serviceAgentFactory).getServiceConfigNetworkResources().releaseMulticastIPAddress(streamNID, sourceNID, relayNID, multicastIP);
    
      }
        //perf-logs  log.addInfo("AddStreamActionRequestHandler:" + request.getUserJID()) + " getResponse - End", MRLog.OK, MRLog.NOTIFY);
  	  	com.ipvs.common.XMPPTransaction.endBlock("AddStreamActionRequest");
		// com.ipvs.common.XMPPTransaction.DEBUG = false;
        return request.createResponseData("actionNID", actionNID);
    	
    }
    
    private String handleAuthorize(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String roomNID, String streamNID,	Element dataEL, Document streamDataDoc)  throws MRException,Exception {
    	
    	String actionData = MRXMLUtils.getValue(dataEL, MediaRoomServiceConst.ACTION_DATA, "");
    	//if auth state allow start stream
    	if(actionData.equals(SessionServiceConst.AUTH_STATE_ALLOW)) {
 			return this.addStreamAction(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamNID, dataEL, MediaRoomServiceConst.START, streamDataDoc);			
 		}
    	
    	int state  = 0;
    	String actionNID = "";
        
    	//if auth state is deny update the state and exist
		if(actionData.equals(SessionServiceConst.AUTH_STATE_DENY)) {			
			 state = MediaRoomServiceConst.DENIED_XAUTH;			 
		} else if(actionData.equals(SessionServiceConst.AUTH_STATE_REVOKE)) {
			//if auth state is revoke then stop stream and update state to revoked
			actionNID = this.addStreamAction(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamNID, dataEL, MediaRoomServiceConst.STOP, streamDataDoc);
			state = MediaRoomServiceConst.REVOKED_XAUTH;
		}
		 
		 MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH, 
          		String.valueOf(state));
		 
		return actionNID;
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	Element data = MediaRoomServiceUtils.cloneDataElement(request);
		
    	String streamNID = MRXMLUtils.getValue(data,"streamNID","");
		com.ipvs.common.XMPPTransaction.startBlock("GetStreamDataDoc");

		String roomNID = MRXMLUtils.getValueXpath(data, ".//roomNID", "");
		if("".equals(roomNID))
			roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);

		Document streamDataDoc  = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
			MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID));
		com.ipvs.common.XMPPTransaction.endBlock("GetStreamDataDoc");
		String xAuthAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//xAuthAgentJID", "");		
		String state = MRXMLUtils.getValueXpath(streamDataDoc, "//Status/state", "");
		String updateAction = MRXMLUtils.getValue(data,"updateAction","");
		    
		if(updateAction.equals(MediaRoomServiceConst.START) && !"".equals(xAuthAgentJID) &&
				Utils.getIntValue(state, MediaRoomServiceConst.STREAM_STOPPED) == MediaRoomServiceConst.STREAM_STOPPED) { 
				MediaRoomServiceUtils.updateStreamState(xmpp, request, db, log, roomNID, streamNID, MediaRoomServiceConst.STREAM_STATE_XPATH,String.valueOf(MediaRoomServiceConst.WAITING_FOR_XAUTH));
				Document responseDoc = request.createResponseData();
	    		MRXMLUtils.setValue(responseDoc.getRootElement(), "WAITING FOR XAUTH");
	    		return responseDoc;
		}    	
       
        Document addStreamActionResponseDoc = addStreamAction(data,request, xmpp, log, db, resourceFactory,serviceAgentFactory);
        return addStreamActionResponseDoc;
    }

	@Override
	public void handleAsynchDBRequestResponse(String dbAction,
			Element responseElement, Object callBackDataObject, int status, String logData, ServiceAgentFactoryI serviceAgentFactory)
			throws Exception {
		if(status != MRRequest.STATE_DONE) {
			MRLog.getInstance(MRLog.CONSOLE).addInfo(callBackDataObject + " Port Instance Failed. LogData:=" +logData);
		}
	}
	
	}
