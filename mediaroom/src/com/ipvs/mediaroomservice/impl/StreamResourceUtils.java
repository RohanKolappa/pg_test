package com.ipvs.mediaroomservice.impl;

import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLAsynchDBUtils;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class StreamResourceUtils {
	
	static StreamResourceUtils theStreamResourceUtils = new StreamResourceUtils();
	public static final String ADD = "ADD";
	public static final String DELETE = "DELETE";
	private final static String MEDIA_STREAM_XML_PATH = "/com/ipvs/xsd/objectXml/MediaStream.xml";


	public synchronized void updateStreamResources(String userJID,XMPPI xmpp, MRDB db, MRLog log, 
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, 
			String action, Element streamElement, String roomID,String roomType,
			AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject)  throws MRException ,Exception {
		String sourcePortNID = MRXMLUtils.getValueXpath(streamElement, ".//sourcePortNID", "");
		String destPortNID = MRXMLUtils.getValueXpath(streamElement, ".//destPortNID", "");
		String relayNID = MRXMLUtils.getValueXpath(streamElement, ".//relayNID", "");
		this.updatePortInstance( userJID, xmpp,  db,  log, 
				 resourceFactory, serviceAgentFactory, sourcePortNID,  action,  streamElement, 
				 roomID, roomType,SessionServiceConst.ROLE_SRC, callBackObject, callBackDataObject);
		this.updatePortInstance(userJID,xmpp, db, log, 
				resourceFactory, serviceAgentFactory, destPortNID,  action,  streamElement,  roomID,
				roomType,SessionServiceConst.ROLE_DST, callBackObject, callBackDataObject);
		if(!"".equals(relayNID))
			this.updatePortInstance(userJID, xmpp, db, log, resourceFactory, serviceAgentFactory, relayNID,  action,  
					streamElement,  roomID, roomType,SessionServiceConst.ROLE_RELAY, callBackObject, callBackDataObject);
       
	}
	
	public synchronized void initPortInstances(String userJID,XMPPI xmpp, MRDB db, MRLog log, 
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, 
			String portNID) {
		try {
			MRRequest request = MRRequest.createRequest(userJID);
			String[] arrWhereXpath = {};
			MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", "", portNID, "//MediaStream", arrWhereXpath);
			log.addInfo("StreamResourceUtils.initPortInstances portNID=" + portNID, MRLog.OK, MRLog.NOTIFY);			
		}
		catch(Exception e) {
			log.addInfo("StreamResourceUtils.initPortInstances error=" + e.toString());
			e.printStackTrace();			
		}
	}
	
	public synchronized void initPortInstancesOfResource(String userJID,XMPPI xmpp, MRDB db, MRLog log, 
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, 
			String objectType) {
		try {
			MRRequest request = MRRequest.createRequest(userJID);
			String[] arrWhereXpath = {};
			MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", objectType, "", "//MediaStream", arrWhereXpath);
			log.addInfo("StreamResourceUtils.initPortInstances objectType=" + objectType, MRLog.OK, MRLog.NOTIFY);			
		}
		catch(Exception e) {
			log.addInfo("StreamResourceUtils.initPortInstances error=" + e.toString());
			e.printStackTrace();			
		}
	}
    
	void addPortInstance(String userJID,XMPPI xmpp, MRDB db, MRLog log, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String portNID, Element streamElement, String roomID,String roomType,String role,
			AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject) throws Exception {
		
		if(portNID==null || portNID.isEmpty()){
			return;
		}
		
		String roomNID = MRXMLUtils.getValueXpath(streamElement, ".//"+SessionServiceConst.ROOMNID, "");
		String streamNID = streamElement.attributeValue("NID");
		String streamID = streamElement.attributeValue(MediaRoomServiceConst.ID);
		String destType = MRXMLUtils.getValueXpath(streamElement,"./data/destType","");
		log.addInfo("StreamResourceUtils.addPortInstance portNID=" + portNID +
				" roomNID=" + roomNID + " streamNID=" + streamNID + 
				" roomID=" + roomID + " streamID=" + streamID, MRLog.OK, MRLog.NOTIFY);		
	
		Element mediaStreamEl= MRXMLUtils.loadXMLResource(MEDIA_STREAM_XML_PATH, MRLog.getInstance(MRLog.CONSOLE)).getRootElement();
		MRXMLUtils.setAttributeValue(mediaStreamEl, AMConst.NID, streamNID);
		MRXMLUtils.setAttributeValue(mediaStreamEl,SessionServiceConst.ID,streamID);
		MRXMLUtils.setAttributeValue(mediaStreamEl, SessionServiceConst.ROLE, role);
		MRXMLUtils.setAttributeValue(mediaStreamEl, SessionServiceConst.ROOMTYPE, roomType);
		MRXMLUtils.setAttributeValue(mediaStreamEl, SessionServiceConst.ROOMNID, roomNID);
		MRXMLUtils.setAttributeValue(mediaStreamEl, SessionServiceConst.ROOMID, roomID);
		
		MRRequest request = MRRequest.createRequest(userJID);
		String mediaStreamXpath = null;
		if(role.equals(SessionServiceConst.ROLE_RELAY)) {
			mediaStreamXpath= "//InstanceInfo/MediaStreamList";			
		} else if(destType.equals(MediaRoomServiceConst.OUTPUT_STREAM) && role.equals(SessionServiceConst.ROLE_DST)) {
			mediaStreamXpath="//InstanceInfo/MediaStreamList";
		} else {
			mediaStreamXpath="//InstanceInfo/MediaStreamList";			
		}
		String[] arrWherExpath = {};
		MRXMLAsynchDBUtils.getInstance().addAsynch(MRXMLDBUtils.TIME_OUT, request, log, db, "", "", portNID, mediaStreamXpath,arrWherExpath , mediaStreamEl,callBackDataObject,callBackObject, serviceAgentFactory);
	}

	void deletePortInstance(String userJID,XMPPI xmpp, MRDB db, MRLog log, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
			String portNID,Element streamElement, String roomID,String roomType, String role,
			AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject) throws Exception {
		if(portNID == null || portNID.isEmpty()){
			return;
		}
		String roomNID = MRXMLUtils.getValueXpath(streamElement, ".//roomNID", "");
		String streamNID = streamElement.attributeValue("NID");
		String streamID = streamElement.attributeValue(MediaRoomServiceConst.ID);
	
		log.addInfo("StreamResourceUtils.deletePortInstance portNID=" + portNID +
				" roomNID=" + roomNID + " streamNID=" + streamNID + 
				" roomID=" + roomID + " streamID=" + streamID, MRLog.OK, MRLog.NOTIFY);		

		MRRequest request = MRRequest.createRequest(userJID);
		String mediaStreamXpath = "//MediaStream[@NID='"+streamNID+"']";
		String[] arrWhereXpath =null;
		MRXMLAsynchDBUtils.getInstance().deleteAsynch(MRXMLDBUtils.TIME_OUT, request, log, db, "", "", portNID, mediaStreamXpath, arrWhereXpath,callBackDataObject,callBackObject, serviceAgentFactory);
		//MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", "", portNID, mediaStreamXpath, arrWhereXpath);
	}

	void updatePortInstance(String userJID,XMPPI xmpp, MRDB db, MRLog log, 
			MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory, 
			String portNID, String action, Element streamElement, String roomID,String roomType,String role,
			AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject) {
		
		try {
			if(StreamResourceUtils.ADD.equals(action))
				this.addPortInstance(userJID,xmpp, db, log, 
						resourceFactory, serviceAgentFactory, portNID,streamElement,  roomID, roomType,role, callBackObject, callBackDataObject);
			else
				this.deletePortInstance(userJID, xmpp,  db,  log, resourceFactory, serviceAgentFactory,
						portNID,streamElement,  roomID, roomType,role,callBackObject, callBackDataObject);
		}
		catch(Exception e) {
			log.addInfo("StreamResourceUtils.updatePortInstance error=" + e.toString());
			e.printStackTrace();
		}
	}		

	
	public static StreamResourceUtils getStreamResourceUtils()  {
		return StreamResourceUtils.theStreamResourceUtils;
	}
	
}
