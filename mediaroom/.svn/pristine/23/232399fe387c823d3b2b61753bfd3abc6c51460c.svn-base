package com.ipvs.mediaroomservice.handler;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.IPVSClientApplet;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.resource.handler.SetDirStorageConfigRequestHandler;
import com.ipvs.systemservice.handler.BaseLoginRequestHandler;



public class GetMediaRoomInvitesRequestHandler extends BaseLoginRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
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
    
    public String updateResourceStateInDB(String contactJID, List<?> resourceNIDList,List<?> mediaDirList, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
    
    	//Element operationEL = null;
        Document resourceDoc = null;
        String mediaStorePortNID = null;
        Hashtable<String, String> updateNidMap = new Hashtable<String, String>();
		
        // send update resource status requests to App Server
        for(Iterator<?> i = resourceNIDList.iterator();i.hasNext();) {
        	String resourceNID = MRXMLUtils.getValue(((Element)i.next()));
        	log.addInfo("GetMediaRoomInvitesRequestHandler: Updating resourceState for contactJID=" + contactJID + " resourceNID=" + resourceNID, MRLog.OK, MRLog.NOTIFY);
            //resourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
            
           	resourceDoc = AppServerAgentUtils.getResourcePresenceManager(serviceAgentFactory).updateState(resourceNID, MRClientUtils.ASSET_STATE_READY, "", contactJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
            //get resource type from response doc. (Eliminates the read document)
           	String resourceType = MRXMLUtils.getElementName(MRXMLUtils.getElementXpath(resourceDoc, "//DBResp/*"), "");
            if(AMConst.MEDIA_STORE_PORT_RESOURCE.equals(resourceType)) {
           	   mediaStorePortNID = resourceNID;
            }
            updateNidMap.put(resourceNID, "Update");
         }    
        if(mediaDirList != null && !mediaDirList.isEmpty()  && mediaStorePortNID !=null){
            Element dirNIDEL = null;
            Document dirDoc = null;
            for(Object object : mediaDirList) {
        	dirNIDEL = (Element) object;
        	dirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(MRXMLUtils.getValue(dirNIDEL), request, db, log);
        	MediaStoreServiceUtils.updateMediaStreamDirResourcePresence(dirDoc, mediaStorePortNID, AMConst.AVAILABLE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);  
            }
        }
        
        // queue port object notifications.
        if(updateNidMap != null){
			new Thread(new PortUpdateNotifier(request, xmpp, log, db, resourceFactory, serviceAgentFactory, updateNidMap)).start();
		}
        
//        // queue port object notifications.
//		((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourcePresenceManager().addPortObjectNotificationsToQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, updateNidMap);
		return mediaStorePortNID;

    }

	/*public void updateResourceContactJIDInMediaRoom(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
		List<?> resourceNIDList, String contactJID) throws Exception {
		
		RoomDB roomDB = RoomDB.getRoomDB();
		
        StringBuffer mediaSourceXPathB = new StringBuffer();
        StringBuffer mediaDestXPathB = new StringBuffer();
        StringBuffer mediaRelayXPathB = new StringBuffer();
        String delim = "";;
       
        for(Iterator<?> i = resourceNIDList.iterator();i.hasNext();) {
        	String resourceNID = MRXMLUtils.getValue(((Element)i.next()));
        	mediaDestXPathB.append(delim + "destPortNID='" + resourceNID + "'");
         	mediaSourceXPathB.append(delim + "sourcePortNID='" + resourceNID + "'");
         	mediaRelayXPathB.append(delim + "relayPortNID='" + resourceNID + "'");       
        	delim = " or ";
        }
        String mediaSourceXPath = "//MediaSource/data[" + mediaSourceXPathB.toString() + "]";
        String mediaDestXPath = "//MediaDest/data[" + mediaDestXPathB.toString() + "]";
        String mediaRelayXPath = "//MediaRelay/data[" + mediaRelayXPathB.toString() + "]";
         
    	// String xpath = "//MediaRoom["+ mediaSourceXPath + " " +  delim + mediaDestXPath +" "+ delim + mediaRelayXPath + " ]";    	
        // Document roomList = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, xpath, false);
        // List<?> myRooms = MRXMLUtils.getListXpath(roomList, "//" + AMConst.MEDIA_ROOM);
       
        // Update the contactJID in each of the rooms
        String mediaSourceNIDXpath = ""; 
        String mediaDestNIDXpath = "";
        String mediaRelayNIDXpath = "";  
        
        ArrayList<String> roomNIDList = RoomDB.getRoomDB().getRoomNIDListForAsset(contactJID);
       	
        for(String roomNID : roomNIDList) {
            StringBuffer streamXpathB  = new StringBuffer();
            delim = "";
        	Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();
        	log.addInfo("GetMediaRoomInvitesRequestHandler:  Updating mediaroom for contactJID=" + contactJID + " roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
        	
        	
        	//update media source,destination and relay contact jid        	
        	roomDB.updateMediaSourceContactJID(request, xmpp, log, db, roomNID, mediaSourceXPath, "sourceAgentJID", contactJID);
        	roomDB.updateMediaDestContactJID(request, xmpp, log, db, roomNID, mediaDestXPath, "destAgentJID", contactJID);
        	roomDB.updateMediaRelayContactJID(request, xmpp, log, db, roomNID, mediaRelayXPath, "relayAgentJID", contactJID);
        	
        
        	//generate stream xpath related to media source , media destination and media relay
        	// String s =  RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc().asXML();
        	
          	mediaSourceNIDXpath = getStreamXpath(MRXMLUtils.getListXpath(roomDoc, mediaSourceXPath),"mediaSourceNID");          
        	mediaDestNIDXpath = getStreamXpath(MRXMLUtils.getListXpath(roomDoc, mediaDestXPath),"mediaDestNID");
        	mediaRelayNIDXpath = getStreamXpath(MRXMLUtils.getListXpath(roomDoc, mediaRelayXPath),"mediaRelayNID");
        	
        	//update stream source agent jid  ,destination agent jid and relay agent jid based on media source and media destination
        	if(!mediaSourceNIDXpath.isEmpty()){
        		streamXpathB.append( delim + mediaSourceNIDXpath );
          		delim = " or ";
          		// mediaSourceNIDXpath = "//Stream/data["  + mediaSourceNIDXpath +" ]";
          		mediaSourceNIDXpath = "//Stream/data["  + mediaSourceNIDXpath + " and (relayNID='')]";

          		roomDB.updateStreamContactJID(request, xmpp, log, db, roomNID, mediaSourceNIDXpath, "sourceAgentJID", contactJID);
          		
          	}
        	if(!mediaDestNIDXpath.isEmpty()){
        		streamXpathB.append( delim + mediaDestNIDXpath );
          		delim = " or ";
          		mediaDestNIDXpath = "//Stream/data["  + mediaDestNIDXpath +" ]";
          		roomDB.updateStreamContactJID(request, xmpp, log, db, roomNID, mediaDestNIDXpath, "destAgentJID", contactJID);          		
          	}
        	
        	if(!mediaRelayNIDXpath.isEmpty()){
        		streamXpathB.append( delim + mediaRelayNIDXpath );
          		delim = " or ";
          		mediaRelayNIDXpath = "//Stream/data["  + mediaRelayNIDXpath +" ]";
          		roomDB.updateStreamContactJID(request, xmpp, log, db, roomNID, mediaRelayNIDXpath, "sourceAgentJID", contactJID);          		
          	}        
            RoomDB.getRoomDB().publishAssetPresenceInRoom(xmpp, log, roomNID, contactJID);
            
            // Get list of all the affected streams
            String streamXpath = streamXpathB.toString();
            if(streamXpath.isEmpty()){
        	    return;  // This should not happen
            } else {
            	streamXpath = "//Stream/data["+ streamXpath +"]";
            }        
            
            // Start all the streams in this room that are waiting for contactJIDS related to this asset
            List<?> streamList = MRXMLUtils.getListXpath(roomDoc, streamXpath);        
	        for(Object o: streamList) {
	        	Element streamDataElement = ((Element)o);
	        	Element streamElement = streamDataElement.getParent();
				int state = MediaRoomServiceConst.STREAM_STOPPED;
				try { 
					state = Integer.parseInt(streamElement.element("Status").element("state").getText());
				}
				catch(Exception e) { e.printStackTrace();}			
				
				String profileNID = streamDataElement.element("profileNID").getText();
				Element profileXML = streamDataElement.element("profileXML");
			
				if(state == MediaRoomServiceConst.WAITING_FOR_DEST) {				
					String destType = MRXMLUtils.getValueXpath(streamElement, "./data/destType", "");
				    if(MediaRoomServiceConst.OUTPUT_RECORD.equals(destType) || MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) {				    	
				    	String streamType = MRXMLUtils.getValueXpath(streamElement, "./data/streamType", "");
				    	String destNID = MRXMLUtils.getValueXpath(streamElement, "./data/destNID", "");
				        MediaRoomServiceUtils.updateDestFileStreamProfile(destNID, streamType, profileXML, roomNID, "","",
				        		xmpp, log, db,request.getUserJID(),request,serviceAgentFactory,resourceFactory);
				        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(destNID, request, log, db);
				        String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
				        if(mediaGroupID.isEmpty()) {
				        	continue;
				        }
				    	Document mediaGroupFileDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db); 
				    	MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.BUSY);
				        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaGroupFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
				        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaGroupFileDoc, mediaStreamFileNID, request.getUserJID(), xmpp, log, db, serviceAgentFactory);
					}
				} else if(state != MediaRoomServiceConst.WAITING_FOR_CONTACTJID) {
					continue;
				}
	
	        	String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
				
				log.addInfo("GetMediaRoomInvitesRequestHandler start streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
				ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, "AddStreamActionRequest", 
						roomNID, streamNID,  profileNID,  profileXML ,MediaRoomServiceConst.START);
	        }
        }
        
        
    }
	
	public String getStreamXpath (List<?> mediaList,String nidXpath) {
		String delim = "";
		StringBuffer mediaNIDXpath = new StringBuffer();
    	for(Object object : mediaList){
    		Element mediaDataEL = (Element) object; 
    		String mediaNID = MRXMLUtils.getAttributeValue(MRXMLUtils.getParentElement(mediaDataEL), "NID", "");
    		mediaNIDXpath.append( delim + nidXpath + "='"+mediaNID +"'");
    		delim = " or ";
    	}
    	return mediaNIDXpath.toString();
	}*/	

        
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document requestDoc = request.getMessage().getDocument();               	
       	
        String contactJID = MRXMLUtils.getValueXpath(requestDoc, "//contactJID", "");
        Element resourceNIDListElement = MRXMLUtils.getElementXpath(requestDoc, "//resourceNIDList");
        List<?> mediaDirNIDList = MRXMLUtils.getListXpath(requestDoc, "//dirNID");
        log.addInfo("GetMediaRoomInvitesRequestHandler: getResponse contactJID=" + contactJID, MRLog.OK, MRLog.NOTIFY);
        // System.out.println("GetMediaRoomInvitesRequestHandler: contactJID=" + contactJID + " resourceNIDList=" + MRXMLUtils.elementToString(resourceNIDListElement));
        if (!contactJID.isEmpty() && (resourceNIDListElement != null) ) {
            List<?> resourceNIDList = resourceNIDListElement.elements();
        	List<String> resourceNIDListString = MRXMLUtils.getValuesXpath(resourceNIDListElement, ".//resourceNID");
            if(resourceNIDList != null && !resourceNIDList.isEmpty()) {
                String mediaStorePortNID = this.updateResourceStateInDB(contactJID, resourceNIDList,mediaDirNIDList,request,xmpp,log,db,resourceFactory,serviceAgentFactory);
                 	
           
                List<?> streamInfoList = MRXMLUtils.getListXpath(requestDoc, "//MediaServiceInfo/.//StreamInfo");
           	
                if(streamInfoList!= null){
                    if (streamInfoList.size() > 0) {
                	for (Object streamInfo : streamInfoList) {
                	    Element streamInfoEle = (Element) streamInfo;
                	    this.handleStreamInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamInfoEle, false);
                	}

                    }
                }           
                streamInfoList = MRXMLUtils.getListXpath(requestDoc, "//LayoutServiceInfo/.//StreamInfo");           
                if(streamInfoList!= null){
                    if (streamInfoList.size() > 0) {        	
                	for (Object streamInfo : streamInfoList) {
                	    Element streamInfoEle = (Element) streamInfo;
                	    this.handleStreamInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamInfoEle, true);
                	}
                    }
                }
        	try {
				updateResourceContactJIDInMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, resourceNIDListString, contactJID);
			} catch (Exception e) {
				e.printStackTrace();
				log.addInfo("Error while recovering streams."+request.getUserJID() , MRLog.INFO,MRLog.NOTIFY);
			}
         	//persistent pvr
         	if(mediaDirNIDList != null && !mediaDirNIDList.isEmpty()  && mediaStorePortNID !=null){
              	Element dirNIDEL = null;
              //	Document dirDoc = null;
      	        for(Object object : mediaDirNIDList) {
      	        	dirNIDEL = (Element) object;
      	        //	dirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(MRXMLUtils.getValue(dirNIDEL), request, db, log);
      	         //   MediaStoreServiceUtils.updateMediaStreamDirResourcePresence(dirDoc, mediaStorePortNID, AMConst.AVAILABLE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);      	        	// Restart/repair persistent PVR sessions	            
      	            String mediaDirNID = MRXMLUtils.getValue(dirNIDEL);	            
      	            AppServerAgent theAppServerAgent = (AppServerAgent) serviceAgentFactory.getServiceAgent(IPVSClientApplet.APPSERVERAGENT);	            
      	            theAppServerAgent.handlePersistentPVRSessionRestart(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaDirNID, false);	    
      	           
      	        }
              }
                 
                /* //TODO gsaxena: auto start session for tx1 and mna devices - will be taken care by DeviceLoginRequestHandler for DI supported devices. to be removed later
                 for (String resourceNID : resourceNIDListString) {
     				Document portResourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
     				//update the cache
     				if (request.getCache() == null )
  					request.setCache(new MRRequestCache());
     				request.getCache().put(resourceNID, portResourceDoc);
     				String portType = MRXMLUtils.getValueXpath(portResourceDoc, "//Info/Type", "");
     				String streamType = MRXMLUtils.getValueXpath(portResourceDoc, "//StreamType", "");
     				if (SessionServiceConst.STREAM_SRC.equals(portType) &&
     						 (MediaRoomServiceConst.RTP_RAW.equals(streamType) || MediaRoomServiceConst.RFB.equals(streamType) || 
     						  MediaRoomServiceConst.RTP_PCM.equals(streamType) || MediaRoomServiceConst.V2D.equals(streamType))) {
     				    	request.setSuperUser(true);
     					AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory,
     							resourceNID, Action.CONTINUE_AUTOSTART_SESSION,"");
     				}
     			}*/
                 Element dirNIDEL = null;
                 for (Object object : mediaDirNIDList) {
                	 dirNIDEL = (Element) object;
                	 String mediaDirNID = MRXMLUtils.getValue(dirNIDEL);
                	 Document dirDoc = MRXMLDBUtils.readElement(request, log, db, mediaDirNID);
                	 SetDirStorageConfigRequestHandler.addDirTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirDoc, mediaDirNID);
                	 String pvrDirNID = MediaStoreServiceUtils.getPVRDirNID(mediaDirNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                	 this.cleanAllPVRFiles(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaDirNID,pvrDirNID);
                 }
            }
        }
    
        return request.createResponseData();
    }
    
    

}
