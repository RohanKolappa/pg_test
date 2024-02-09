package com.ipvs.notification;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.http.common.HttpAuthentication.AuthScheme;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.subscriptions.SubscriptionManager;
import com.ipvs.utils.Utils;


public class ResourceManager{
	
	@SuppressWarnings("unused")
	private MRLog log =null;
	private ResourceNotifier resourceNotifier = null;
	public ResourceManager(MRDB db,MRLog log,XMPPF xf, String userJID, SubscriptionManager subscriptionManager,AuthTokenCache authTokenCache) {
		this.log = log;
		resourceNotifier  = new ResourceNotifier(db,log, xf, userJID, subscriptionManager,authTokenCache);
    	Thread resourceNotifierThread = new Thread(resourceNotifier);
    	resourceNotifierThread.start();
	}	
	
	public void queueConnectionNotification(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String srcNID, String dstNID,  
			String sessionID, String connID, Document sessionDoc, Element mediaSourceEL, Element streamElement, List<Element> observerList, String action) {

		/* <objectTypeConnectionUpdate>
        	<objectType NID = "" operation = "ADD/DELETE" > 
        	  <connectionInfo>
        	  </connectionInfo>
        	<objectType>
         </objectTypeConnectionUpdate>
		 */
		try {
			String srcObjectType = MRXMLUtils.getValueXpath(mediaSourceEL, ".//sourceType","");
			String dstObjectType = MRXMLUtils.getValueXpath(streamElement, ".//destType","");
			String mediaType = "";

			if(MediaRoomServiceConst.INPUT_ENCODER.equals(srcObjectType)) {
                srcObjectType = SessionServiceConst.PORT;
                mediaType = "live";
            } else {
                srcObjectType = SessionServiceConst.FILE;
                mediaType = "recording";
            }

			if((MediaRoomServiceConst.OUTPUT_DECODER.equals(dstObjectType) ||
					MediaRoomServiceConst.OUTPUT_STREAM.equals(dstObjectType))) {
                dstObjectType = SessionServiceConst.PORT;
            } else {
                dstObjectType = SessionServiceConst.FILE;
            }
		
			if(!srcNID.isEmpty()){
				Element connInfoEl = generateConnectionElement(xmpp, request, log, db, srcNID, sessionID, connID, sessionDoc, mediaSourceEL, streamElement, observerList, action,
						srcObjectType);
				queueNotificationForBroadcast(xmpp, request, log, db, srcObjectType, SessionServiceConst.CONNECTION_UPDATE, srcNID, action, connInfoEl, false, true, (srcNID + "." + connID +"."+SessionServiceConst.CONNECTION_UPDATE));
			}
							
			if(!dstNID.isEmpty()){
				Element connInfoEl = generateConnectionElement(xmpp, request, log, db, srcNID, sessionID, connID, sessionDoc, mediaSourceEL, streamElement, observerList, action,
					dstObjectType);
				if(dstObjectType.equals(SessionServiceConst.PORT)){
					String timeStamp =String.valueOf(xmpp.getCurrentTimeMillis() + (SessionServiceResourceConst.API_KEY_VALID_INTERVAL * 1000));

					String thumbnailURL = HttpAuthentication.generateThumbnailURL(srcNID, mediaType, timeStamp, "127.0.0.1",AuthScheme.EXPIRY.toString(), log, null, MRRequest.createRequest(((MRRequest) request).getAppServerJID()),db,xmpp,null,"");
					if(!thumbnailURL.isEmpty()){
						MRXMLUtils.addAttribute(MRXMLUtils.getElementXpath(connInfoEl, ".//"+dstObjectType + SessionServiceConst.CONNECTIONINFO), SessionServiceResourceConst.THUMBNAILURL_ATTRIBUTE,thumbnailURL);
					}
				}
				queueNotificationForBroadcast(xmpp, request, log, db, dstObjectType, SessionServiceConst.CONNECTION_UPDATE, dstNID, action, connInfoEl, false, true, (dstNID + "." + connID + "."+SessionServiceConst.CONNECTION_UPDATE));
			}
		}
		catch(MRException me) {
			log.addInfo("queueConnectionNotification" + action + "failed " + "MRException=" + me.getData(), MRLog.ERROR, MRLog.NOTIFY);
		} 
		catch(Exception e) {
			log.addInfo("queueConnectionNotification" + action + "failed " + "Exception=" + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
		} 
	}

	private Element generateConnectionElement(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String srcNID, String sessionID, String connID, Document sessionDoc,
			Element mediaSourceEL, Element streamElement, List<Element> observerList, String action, String objectType) throws MRException, Exception {
		Element connInfoEl = generateElementForConnectionInfo(request, xmpp, log, db, sessionDoc, sessionID, connID, objectType);
		if(SessionServiceConst.FILE.equalsIgnoreCase(objectType)) {
            observerList = SessionServiceUtils.getMonitorAgentList(mediaSourceEL, SessionServiceConst.FILE, log);
        }
		if(observerList != null && !observerList.isEmpty()) {
			connInfoEl = fillExtendedConnectionFormatInfo(connInfoEl, streamElement, mediaSourceEL, sessionDoc, connID, db, log);
			Element objectTypeConnUpdateEl = createNotificationDocument(objectType, SessionServiceConst.CONNECTION_UPDATE, srcNID, action,connInfoEl,false,false);
			queueObserversConnectionNotifications(xmpp,db, objectTypeConnUpdateEl, observerList);
		}
		return connInfoEl;
	}
	
	private void queueNotificationForBroadcast(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String objectType,String notificationName,String nid,String action,Element childElement,boolean donotAddNIDElement,boolean addSubscriptionID, String notificationId) throws Exception {
		
		Element notificationEl = createNotificationDocument(objectType, notificationName, nid, action, childElement, donotAddNIDElement, addSubscriptionID);
		String messageXML = SessionServiceUtils.getSessionMessageNotification(notificationEl);
		Element notificationRequestDataEl = MRXMLUtils.newElement("NotificationRequestData");
		MRXMLUtils.setAttributeValue(notificationRequestDataEl, SessionServiceConst.NID, nid);
		MRXMLUtils.setAttributeValue(notificationRequestDataEl, SessionServiceConst.TYPE_ATTRIBUTE, notificationEl.getName());
		String notificaitonRequestDataString  = MRXMLUtils.elementToString(notificationRequestDataEl);
		resourceNotifier.queueNotification(ResourceNotifier.SUBSCRIBER_LIST_NOTIFICATION_TYPE,notificaitonRequestDataString, notificationId, messageXML, null);
	}
	
	private void queueObserversConnectionNotifications(XMPPI xmpp, MRDB db, Element messageEl, List<Element> observerList) throws MRException, Exception{
		for (Object object1:observerList){
			Element observerInfoEl = (Element)object1;
			String JID = MRXMLUtils.getAttributeValue(observerInfoEl, "JID");
			String messageXML = SessionServiceUtils.getSessionMessageNotification(messageEl);
			resourceNotifier.queueNotification(ResourceNotifier.JID_NOTIFICATION_TYPE,JID, Utils.getUUID(), messageXML, null);
		}
	}
	public void queueConnectionStateNotification(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String srcNID, String dstNID, 
			String srcObjectType, String dstObjectType, String sessionID, Element connStateEl, List<Element> observerList, String connId) {
		try {
			if(MediaRoomServiceConst.INPUT_ENCODER.equals(srcObjectType)) {
                srcObjectType = SessionServiceConst.PORT;
            } else {
                srcObjectType = SessionServiceConst.FILE;
            }

			if((MediaRoomServiceConst.OUTPUT_DECODER.equals(dstObjectType) ||
					MediaRoomServiceConst.OUTPUT_STREAM.equals(dstObjectType))) {
				dstObjectType = SessionServiceConst.PORT;
			}
			else { 
				dstObjectType = SessionServiceConst.FILE;
			}

			Element connectionStateEL = (Element)connStateEl.clone();
			connectionStateEL.setName("ConnectionState");

			if(observerList !=null && !observerList.isEmpty()) {
				Element objectTypeConnUpdateEl = createNotificationDocument(srcObjectType, SessionServiceConst.CONNECTION_STATE_UPDATE, srcNID, "",connectionStateEL,false,false);
				queueObserversConnectionNotifications(xmpp,db, objectTypeConnUpdateEl, observerList);
			}

			if(!srcNID.isEmpty()) {
                queueNotificationForBroadcast(xmpp, request, log, db, srcObjectType, SessionServiceConst.CONNECTION_STATE_UPDATE, srcNID, "", connectionStateEL, false, true, (srcNID + "." + connId +"."+SessionServiceConst.CONNECTION_STATE_UPDATE));
            }

			if(!dstNID.isEmpty()) {
                queueNotificationForBroadcast(xmpp, request, log, db, dstObjectType, SessionServiceConst.CONNECTION_STATE_UPDATE, dstNID, "", connectionStateEL, false, true, (dstNID + "." + connId + "."+SessionServiceConst.CONNECTION_STATE_UPDATE));
            }
		}
		catch(Exception e) {
			log.addInfo("queueConnectionStateNotification ConnectionStateUpdate failed " + "Exception=" + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
		}
	}
	
	public void queueObjectNotification(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String  objectType,String nodeName, String nid,String action,Document messageDoc) {
		try {
			String notificationId  = nid + "." +  action;
			queueNotificationForBroadcast(xmpp, request, log, db, objectType, SessionServiceConst.UPDATE, nid, action, MRXMLUtils.getElementXpath(messageDoc, "//"+nodeName ), true, true, notificationId);
		}
		catch(Exception e){
			log.addInfo("queueObjectNotification Update failed " + "Exception=" + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
		}
	}	
	
	private Element createNotificationDocument(String objectType,String notificationName,String nid,String action,Element childElement,boolean donotAddNIDElement,boolean addSubscriptionID) throws Exception {
		String notificationElName = objectType + notificationName ;		
		Element objectTypeUpdateEl = MRXMLUtils.newElement(notificationElName);
		Element objectTypeEl = null;
		if(!donotAddNIDElement) {
			objectTypeEl = MRXMLUtils.newElement(objectType);			
			MRXMLUtils.setAttributeValue(objectTypeEl, "NID", nid);
			MRXMLUtils.addElement(objectTypeEl, (Element)childElement.clone());			
		}  else {
			objectTypeEl = (Element)childElement.clone();
		}
		
		if(addSubscriptionID) {
			MRXMLUtils.setAttributeValue(objectTypeEl, SessionServiceConst.SUBSCRIPTION_ID, SessionServiceConst.SUBSCRIPTION_ID_TOKEN);			
		}
		
		MRXMLUtils.addElement(objectTypeUpdateEl, objectTypeEl);
		if(!action.isEmpty()) {
            MRXMLUtils.setAttributeValue(objectTypeEl, "operation", action);
        }
		
		MRXMLUtils.setAttributeValue(objectTypeEl, "dirty", MRXMLUtils.getAttributeValue(childElement, "dirty", "false"));

		return objectTypeUpdateEl;
	}
	
	
	//connection update notifications
	public Element generateAuthConnectionInfoElement(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, 
			Element connEl,String sessionId, Document sessionDoc, Element mediaSourceEl,
	         Element streamEl) throws MRException,Exception {

		Document connectionListDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.CONNECTION_LIST_INSTANCE_TEMPLATE, log);
		Element connectionInfoEL = (Element)MRXMLUtils.getElementXpath(connectionListDoc,"//"+ SessionServiceConst.AUTHCONNECTIONINFO).detach();
		Element dstEntityEl = null;
		Element srcEntityEl = null;
		Element connSrcEntityEl = null;
		Element connDstEntityEl = null;
		Element profileEl =null;
		String srcEntityId = "";
		String dstEntityId = "";
		String  ownerJID = "";
		String profileId = "";
		srcEntityId = MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.SRCENTITYID,"");
		dstEntityId = MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.DSTENTITYID,"");
		profileId = MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.PROFILE_ID,"");
		dstEntityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[id='"+MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.SRCENTITYID)+"']");
		
		connectionInfoEL = setConnectionInfoElement(connectionInfoEL, connEl);

		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.SESSION_ID, sessionId);
		
		srcEntityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityId+"']");
		connSrcEntityEl = MRXMLUtils.getElement(connectionInfoEL,SessionServiceConst.SRC_ENTITY);

		MRXMLUtils.setAttributeValue(connSrcEntityEl, SessionServiceConst.ID, MRXMLUtils.getAttributeValue(srcEntityEl, SessionServiceConst.ID,""));
		MRXMLUtils.setAttributeValue(connSrcEntityEl, SessionServiceConst.RESOURCENID, MRXMLUtils.getAttributeValueXpath(srcEntityEl, "./Port" ,SessionServiceConst.RESOURCENID,""));
		MRXMLUtils.setAttributeValue(connSrcEntityEl, SessionServiceConst.RESOURCETITLE, MRXMLUtils.getAttributeValueXpath(srcEntityEl, "./Port" ,SessionServiceConst.RESOURCETITLE,""));
		ownerJID = MRXMLUtils.getValueXpath(mediaSourceEl, ".//sourceAgentJID", "");
		
		connSrcEntityEl = fillSrcDstUserInfo(ownerJID, connSrcEntityEl, db, log);
		
		dstEntityEl = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+dstEntityId+"']");
		connDstEntityEl = MRXMLUtils.getElement(connectionInfoEL,SessionServiceConst.DST_ENTITY);

		MRXMLUtils.setAttributeValue(connDstEntityEl, SessionServiceConst.ID, MRXMLUtils.getAttributeValue(dstEntityEl, SessionServiceConst.ID,""));
		MRXMLUtils.setAttributeValue(connDstEntityEl, SessionServiceConst.RESOURCENID, MRXMLUtils.getAttributeValueXpath(dstEntityEl,"./Port" ,SessionServiceConst.RESOURCENID,""));
		MRXMLUtils.setAttributeValue(connDstEntityEl, SessionServiceConst.RESOURCETITLE, MRXMLUtils.getAttributeValueXpath(dstEntityEl,"./Port", SessionServiceConst.RESOURCETITLE,""));
		ownerJID = MRXMLUtils.getValueXpath(streamEl, ".//destAgentJID", "");
		
		connDstEntityEl = fillSrcDstUserInfo(ownerJID, connDstEntityEl, db, log);
	
		profileEl = MRXMLUtils.getElement(connectionInfoEL,SessionServiceConst.PROFILE);
		profileEl = setProfileInfoElement(streamEl, profileId, profileEl);
		
		return connectionInfoEL;
	}
	
	public Element generateConnectionInfoElement(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, String sessionId, 
			String connId,Document sessionDoc,Element mediasourceEl,Element streamElement) throws MRException,Exception {		
		Element connEl = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+connId+"']");
		return generateAuthConnectionInfoElement(request, xmpp, log, db, connEl, sessionId, sessionDoc, mediasourceEl,streamElement);
	}
	
	
	public Element generateAuthConnectionInfoElement(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, 
			Element connEl, String sessionId, Document sessionDoc, String roomNID) throws MRException,Exception {		
		Document roomDoc =MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);
		Element streamElement = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id='"+MRXMLUtils.getAttributeValue(connEl,SessionServiceConst.ID)+"']");
		String mediaSourceNID = MRXMLUtils.getValueXpath(streamElement, ".//mediaSourceNID", "");
		Element mediaSourceEl = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@NID='" + mediaSourceNID + "']");
		return generateAuthConnectionInfoElement(request, xmpp, log, db, connEl, sessionId, sessionDoc, mediaSourceEl, streamElement);
	}
	
	
	public Element generateElementForConnectionInfo(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, Document sessionDoc, String sessionId, 
			String connId, String objectType) throws MRException,Exception {
		
		Element connEl = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+connId+"']");
		Element sessionEL = connEl.getParent().getParent();
		Element sessionInfoEl = generateSessionInfoElement(sessionEL, objectType);
		Element connectionInfoEl = generateBasicConnectionInfoElement(sessionEL, connEl, request, log, db, objectType);
		MRXMLUtils.addElement(sessionInfoEl, connectionInfoEl);
		Element connectionStatusEl = MRXMLUtils.newElement(SessionServiceConst.CONNECTION_STATUS);
		MRXMLUtils.addElement(connectionStatusEl, (Element)sessionInfoEl.detach());
		//return getConnectionInfoElement(request, xmpp, log, db, connEl, sessionId, sessionDoc, mediasourceEl,streamElement);
		return connectionStatusEl;
	}
	
	public Element generateSessionInfoElement(Element sessionEL, String objectType) throws Exception {
		Element sessionInfoEL = MRXMLUtils.newElement(objectType + SessionServiceConst.SESSIONINFO);
		String sessionId = MRXMLUtils.getAttributeValue(sessionEL, SessionServiceConst.ID, "");
		MRXMLUtils.setAttributeValue(sessionInfoEL, SessionServiceConst.ID, sessionId);
		MRXMLUtils.setAttributeValue(sessionInfoEL, SessionServiceConst.NAME, MRXMLUtils.getAttributeValue(sessionEL, SessionServiceConst.NAME, ""));
		return sessionInfoEL;
	}
	
	public Element setConnectionInfoElement(Element connectionInfoEL, Element connEl) throws Exception {
		String connId = MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.ID);
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.ID, connId);
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.TYPE_ATTRIBUTE, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.TYPE_ATTRIBUTE,""));
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.NAME, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.NAME,""));
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.STATE, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.STATE,""));
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.WINDOWID, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.WINDOWID,""));
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.CANVASID, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.CANVASID,""));
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.ERROR_CODE, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.ERROR_CODE,""));
		MRXMLUtils.setAttributeValue(connectionInfoEL, SessionServiceConst.ERROR_DATA, MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.ERROR_DATA,""));
		return connectionInfoEL;
	}
	
	public Element setProfileInfoElement(Element streamEl, String profileId, Element profileEl) throws Exception {
		Element profileXMLEl = MRXMLUtils.getElementXpath(streamEl,".//profileXML");
		if(profileXMLEl == null) {
            return profileEl;
        }
		Document profileDoc = MRXMLUtils.elementToDocument(profileXMLEl);
		MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.ID, profileId);
		MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.PROFILE_NAME, MRXMLUtils.getValueXpath(profileDoc, "//Title", ""));
		MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.PROFILE_NID, MRXMLUtils.getValueXpath(streamEl, ".//profileNID", ""));
		String streamType = MRXMLUtils.getValueXpath(streamEl, ".//streamType", "");				
		if(streamType.equals(MediaRoomServiceConst.V2D)){					
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.ALLOW_KM_SWITCH, MRXMLUtils.getValueXpath(profileDoc, "//AllowKMSwitch", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.REMOTE_KM, MRXMLUtils.getValueXpath(profileDoc, "//EnableKbm", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.AVOPTION, MRXMLUtils.getValueXpath(profileDoc, "//AVOption", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.MAX_BANDWIDTH, MRXMLUtils.getValueXpath(profileDoc, "//Bandwidth", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.LOW_COMPRESSION, MRXMLUtils.getValueXpath(profileDoc, "//Compression/Low", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.HIGH_COMPRESSION, MRXMLUtils.getValueXpath(profileDoc, "//Compression/High", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.BURST_RATIO, MRXMLUtils.getValueXpath(profileDoc, "//BurstRatio", ""));
			MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.BURST_DURATION, MRXMLUtils.getValueXpath(profileDoc, "//BurstDuration", ""));
			boolean multicast = Utils.getBooleanValue(MRXMLUtils.getValueXpath(profileDoc, "//IsMulticast", ""),false);
			if(multicast) {
				MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.MULTICAST, "true");
				MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.UNICAST, "false");
			} else {
				MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.MULTICAST, "false");
				MRXMLUtils.setAttributeValue(profileEl, SessionServiceConst.UNICAST, "true");
			}
		}
		return profileEl;
	}
	
	public Element generateBasicConnectionInfoElement(Element sessionEL, Element connEL, MRRequestI request, MRLog log, MRDB db, String objectType) throws Exception {
		Element portConnectionInfoEL = MRXMLUtils.newElement(objectType + SessionServiceConst.CONNECTIONINFO);
		portConnectionInfoEL = setConnectionInfoElement(portConnectionInfoEL, connEL);
		String srcResourceType = "./"+objectType;
		String dstResourceType = "./"+objectType;
		
		String srcEntityId = MRXMLUtils.getAttributeValue(connEL, SessionServiceConst.SRCENTITYID,"");
		Element srcEntityEl = MRXMLUtils.getElementXpath(sessionEL, ".//Entity[@id='"+srcEntityId+"']");

		String dstEntityId = MRXMLUtils.getAttributeValue(connEL, SessionServiceConst.DSTENTITYID,"");
		Element dstEntityEl = MRXMLUtils.getElementXpath(sessionEL, ".//Entity[@id='"+dstEntityId+"']");
		
		String srcEntityType =  MRXMLUtils.getAttributeValue(srcEntityEl, SessionServiceConst.TYPE_ATTRIBUTE, "");
		String dstEntityType =  MRXMLUtils.getAttributeValue(dstEntityEl, SessionServiceConst.TYPE_ATTRIBUTE, "");
		
		if(srcEntityType.equals(SessionServiceConst.PLYFILE)) {
			srcResourceType = "./File";
		}
		
		if(dstEntityType.equals(SessionServiceConst.PVRFILE) || dstEntityType.equals(SessionServiceConst.RECFILE)) {
			dstResourceType = "./File";
		}
		
		MRXMLUtils.setAttributeValue(portConnectionInfoEL, SessionServiceConst.DSTNID, MRXMLUtils.getAttributeValueXpath(dstEntityEl,dstResourceType ,SessionServiceConst.RESOURCENID, ""));
		MRXMLUtils.setAttributeValue(portConnectionInfoEL, SessionServiceConst.DSTTITLE, MRXMLUtils.getAttributeValueXpath(dstEntityEl,dstResourceType ,SessionServiceConst.RESOURCETITLE, ""));
		MRXMLUtils.setAttributeValue(portConnectionInfoEL, SessionServiceConst.SRCNID, MRXMLUtils.getAttributeValueXpath(srcEntityEl,srcResourceType ,SessionServiceConst.RESOURCENID, ""));
		MRXMLUtils.setAttributeValue(portConnectionInfoEL, SessionServiceConst.SRCTITLE, MRXMLUtils.getAttributeValueXpath(srcEntityEl,srcResourceType ,SessionServiceConst.RESOURCETITLE, ""));				    
		
		return portConnectionInfoEL;
}
	
	public Element fillSrcDstUserInfo(String ownerJID, Element connEl, MRDB db, MRLog log) throws Exception{
		Element userEl = DBServiceUtils.getUser(MRRequest.createRequest(ownerJID), db, log);
		if(userEl !=null) {
			MRXMLUtils.setAttributeValue(connEl, SessionServiceConst.USERJID, ownerJID);
			MRXMLUtils.setAttributeValue(connEl, SessionServiceConst.USER_FULL_NAME, MRXMLUtils.getValueXpath(userEl, ".//Name", ""));
		}
		return connEl;
	}
	
	public Element fillExtendedConnectionFormatInfo(Element connectionInfoEL, Element streamEl, Element mediaSourceEl, Document sessionDoc, String connId, MRDB db, MRLog log) throws Exception{

		Element profileEl = null;
		String srcOwnerJID ="";
		String dstOwnerJID ="";
		String profileId ="";
		Element connSrcEntityEl = null;
		Element connDstEntityEl = null;
		
		srcOwnerJID = MRXMLUtils.getValueXpath(mediaSourceEl, ".//sourceAgentJID", "");
		connSrcEntityEl = MRXMLUtils.getElement(connectionInfoEL,SessionServiceConst.SRC_ENTITY);
		
		fillSrcDstUserInfo(srcOwnerJID, connSrcEntityEl, db, log);
		
		dstOwnerJID = MRXMLUtils.getValueXpath(streamEl, ".//destAgentJID", "");
		connDstEntityEl = MRXMLUtils.getElement(connectionInfoEL,SessionServiceConst.DST_ENTITY);
		
		fillSrcDstUserInfo(dstOwnerJID, connDstEntityEl, db, log);
		Element connEl = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+connId+"']");
		profileId = MRXMLUtils.getAttributeValue(connEl, SessionServiceConst.PROFILE_ID,"");
		profileEl = MRXMLUtils.getElement(connectionInfoEL,SessionServiceConst.PROFILE);
		if(profileEl == null) {
			profileEl = MRXMLUtils.newElement(SessionServiceConst.PROFILE);
		}
		setProfileInfoElement(streamEl, profileId, profileEl);
		MRXMLUtils.addElement(connectionInfoEL, (Element)profileEl.clone());
		return connectionInfoEL;
	}
 
	public void reset() {
		this.resourceNotifier.setReset(true);
		this.resourceNotifier = null;
	}
	
	public void queueDisplayNotification(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, String  objectType,String nodeName,String portNID, Element displayStatusEl) {
//		
//		MRXMLUtils.setAttributeValue(displayStatusEl, SessionServiceConst.NID, portNID);
//		Element notificationEl = createNotificationDocument(objectType, nodeName + SessionServiceConst.UPDATE, portNID,
//				SessionServiceConst.UPDATE, displayStatusEl,true,true);
//		String messageXML = SessionServiceUtils.getSessionMessageNotification(MRXMLUtils.elementToDocument(notificationEl));
//		Element notificationRequestDataEl = MRXMLUtils.newElement("NotificationRequestData");
//		MRXMLUtils.setAttributeValue(notificationRequestDataEl, SessionServiceConst.NID, portNID);
//		MRXMLUtils.setAttributeValue(notificationRequestDataEl, SessionServiceConst.TYPE_ATTRIBUTE, notificationEl.getName());
//		String notificaitonRequestDataString  = MRXMLUtils.elementToString(notificationRequestDataEl);
//		resourceNotifier.queueNotification(ResourceNotifier.SUBSCRIBER_LIST_NOTIFICATION_TYPE,notificaitonRequestDataString, Utils.getUUID(), messageXML);
//		
//		
		try {
			String notificationId  = portNID + "." +  nodeName + "." + SessionServiceConst.UPDATE;
			MRXMLUtils.setAttributeValue(displayStatusEl, SessionServiceConst.NID, portNID);		
			queueNotificationForBroadcast(xmpp, request, log, db, objectType, nodeName+SessionServiceConst.UPDATE, portNID,  SessionServiceConst.UPDATE, displayStatusEl , true, true, notificationId);
		}
		catch(Exception e) {
			log.addInfo("queueDisplayNotification Update failed " + "Exception=" + e.toString(), MRLog.ERROR, MRLog.NOTIFY);
		}
	}
}

