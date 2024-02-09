package com.ipvs.mediaroomservice.handler;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.resource.handler.DeleteFileRequestHandler;


public class DeleteMediaRoomRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
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

    // TBD return a list of streamNIDs for the room
    public ArrayList<String> getStreamNIDList(Document roomdoc) throws Exception {
        ArrayList<String> result = new ArrayList<String>();

        List<?> list = MRXMLUtils.getListXpath(roomdoc, "//Stream");

        Iterator<?> i = list.iterator();

        while (i.hasNext()) {
            Element streamElement = (Element) i.next();
            String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
            result.add(streamNID);
        }

        return result;
    }

    protected void cleanRoom(String roomNID, Element data, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        // MRXMLDBUtils.validateElement(request, log, db, roomNID, "mediaroomservice"); // TBR
        //perf-logs log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID()) + " MRXMLDBUtils.readElement(request, log, db, roomNID) - Start", MRLog.OK, MRLog.NOTIFY);
        Document doc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);

        //perf-logs log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID()) + " MRXMLDBUtils.readElement(request, log, db, roomNID) - End", MRLog.OK, MRLog.NOTIFY);	  
        Element mroot =  (request.getMessage()).getDocument().getRootElement();

        ArrayList<String> streamNIDList = getStreamNIDList(doc);
        Iterator<String> i = streamNIDList.iterator();
        log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID() + "/" + roomNID + " Checking room found #streams=" + streamNIDList.size());

        // AddStreamAction(Stop) to each stream
        while (i.hasNext()) {
            String streamNID = i.next();
            log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID() + "/" + streamNID + " Checking if stream stopped");
            log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID() + "/" + roomNID + " Stopping streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);

            Element rqdata = MRXMLUtils.stringToElement("<data><DeleteMediaRoomRequestData>" + "<streamNID>" + streamNID + "</streamNID>" + "<updateAction>" + MediaRoomServiceConst.STOP +
                    "</updateAction>" + "</DeleteMediaRoomRequestData></data>");
            MRXMLUtils.removeChild(mroot,"data");
            mroot.add(rqdata);
            
            try {
                MRRequestHandlerI handler = new AddStreamActionRequestHandler();
                //Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        // Send UnWatch to the destinations
        Iterator<String> s = streamNIDList.iterator();

        while (s.hasNext()) {
            String streamNID = s.next();

            try {
                String streamData = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID);
                Document streamDataDoc = MRXMLUtils.stringToDocument(streamData);
                String name = MRConst.LAYOUT_SERVICE;
                Element destAgentJIDElement = MRXMLUtils.getElementXpath(streamDataDoc, "//destAgentJID");
                ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, roomNID, AppServerAgent.class.getName(), MRXMLUtils.getValue(destAgentJIDElement), name, MediaRoomServiceConst.TEARDOWNREQUEST,
                    "<data><streamNID>" + streamNID + "</streamNID></data>",
                    "DeleteMediaRoomRequestHandler:=" + request.getUserJID() + "/" + streamNID + " > Teadrdown Message to " +
                    ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.DESTSTREAMAGENT), request.getNID(),request.getAppServerJID(),MRConst.LAYOUT_SERVICE,"");
            } catch (Exception e) {
                log.addInfo("Error sending Teardown for streamID=" + streamNID);
            }
        }
    }

    // Check if we need to gc PVR files
    protected void gcPVRFiles(String roomNID, Document roomdoc, MRRequest request, XMPPI xmpp, 
    		MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	 List<?> list = MRXMLUtils.getListXpath(roomdoc, "//PVRTrack//Stream/data/destNID");
         Iterator<?> i = list.iterator();
        
        // set the requested user as Appserver not the destination user.
         System.out.println("request.getServiceName()="+request.getServiceName());
        MRRequest assetRequest = MRRequest.createRequest(request.getAppServerJID(), request.getLogLevel(), request.getNID(), request.getServiceName(), request.getRequestName(), new MRMessage(request.getMessage().getDocument()), request.getResourceGroup(), request.getAppServerJID(), request.getUserAgentName(), request.getClientData());
        while (i.hasNext()) {
        	Element destNIDElement = (Element)i.next();
        	String fileNID = MRXMLUtils.getValue(destNIDElement);
	        try {
		        log.addInfo("DeleteMediaRoomRequestHandler:gcPVRFiles Begin fileNID=" + fileNID, MRLog.OK, MRLog.NOTIFY);		        		 
		        assetRequest.setSuperUser(true);
		        Document mediaStreamFileDoc = MRXMLDBUtils.readElement(assetRequest, log, db, fileNID);
		        new DeleteFileRequestHandler().deleteFile(mediaStreamFileDoc, assetRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, new DeleteFileRequestHandler().getClass().getName());
		          
		        log.addInfo("DeleteMediaRoomRequestHandler:gcPVRFiles End fileNID=" + fileNID, MRLog.OK, MRLog.NOTIFY);
	        }
	        catch(Exception e) {
	        	// e.printStackTrace();
		        log.addInfo("DeleteMediaRoomRequestHandler:gcPVRFiles Failed fileNID=" + fileNID + " Exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
	        }
        }
    }

    /* List of External Events and the corresponding roomGC
     * Extenal Action  => SourceAgent                                                            DestAgent
     * Browser Closed  => OwnerUnavailable(DeleteAllStreams)                OwnerUnavailable(DeleteAllStreams)
     * Source Down         => -                                                                                SourceUnavailable(DeleteAllStreams)
     * Relay Down                 => OwnerUnavailable(Upstream Tx)                        SourceUnavailable(Downstream Viewer)
     */
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        //perf-logs log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID()) + " getResponse - Start", MRLog.OK, MRLog.NOTIFY);

        // Check the data
        Element data = MediaRoomServiceUtils.cloneDataElement(request);
        String roomNID = MRXMLUtils.getValue(data,"roomNID","");
        
        Document roomdoc = null;
        try {
        	roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
        	MediaRoomServiceUtils.deleteElement(roomNID, xmpp, request, log, db, roomNID);
        }
        catch(Exception e) {
        	//throw new MRException(MediaRoomServiceExceptionCodes.ROOMNOTFOUND, roomNID);
        	log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID() + "/" + roomNID + " DeleteChatRoom Exception=" + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
        }
        log.addInfo("DeleteMediaRoomRequestHandler:getResponse: done deleteMediaRoomNode", MRLog.OK, MRLog.NOTIFY);
        
        List<?> streamList = MRXMLUtils.getListXpath(roomdoc, "//Stream");

        //Release multicast ipaddress
        for(Object o : streamList) {
        	Element streamElement = (Element)o;
        	String streamNID = MRXMLUtils.getAttributeValue(streamElement, "NID","");
        	String sourceNID = MRXMLUtils.getValueXpath(streamElement, ".//sourceNID", "");
        	String relayNID = MRXMLUtils.getValueXpath(streamElement, ".//relayNID", "");
        	String multicastIP = MRXMLUtils.getValueXpath(streamElement, ".//multicastIP", "");
        	AppServerAgentUtils.getServiceConfigManager(serviceAgentFactory).getServiceConfigNetworkResources().releaseMulticastIPAddress(streamNID, sourceNID, relayNID, multicastIP);
        	
        }
    
        String roomGC = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//roomGC", "");
        
        //sendUpdateRecordingFilePlayLengthAndState(roomNID,roomdoc,request,xmpp,log,db,resourceFactory,serviceAgentFactory);
        
        if(MRConst.TRUE.equals(roomGC) && roomdoc!=null)
        	this.gcPVRFiles(roomNID, roomdoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

       

        //perf-logs log.addInfo("DeleteMediaRoomRequestHandler:=" + request.getUserJID()) + " getResponse - End", MRLog.OK, MRLog.NOTIFY);
        return request.createResponseData("roomNID", roomNID);
    }  	
}
