package com.ipvs.systemservice.handler;

import java.util.Enumeration;
import java.util.Hashtable;

import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.IPVSScheduler;
import com.ipvs.common.IPVSTask;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;

public class RoomGC extends IPVSTask {
     //How to make this timer adjustable to number of devices as this class is only instantiated in AppServerAgentUtils once??
    
    //Hashtable<String, Hashtable <String, String>> roomGCList;
    Hashtable<String, StreamData> roomGCList;
    MRRequest request;
    MRDB db;
    MRLog log;
    private int totalDelay;
    static final int addedDelay= 60;
    public RoomGC(MRDB db, MRLog mrLog, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {
		super(null, xmpp, mrLog, db, resourceFactory, serviceAgentFactory, "RoomGC");
		//roomGCList = new Hashtable<String, Hashtable <String, String>>();
		roomGCList = new Hashtable<String, StreamData>();
		this.log = mrLog;
		this.db = db;
		totalDelay = MediaRoomServiceConst.GC_SCHEDULE_INTERVAL;

	//IPVSScheduler.getInstance().scheduleBySeconds(this, GC_SCHEDULE_INTERVAL);
    }    

    protected synchronized void addTime(int newDelay){
   	IPVSScheduler.getInstance().removeTask(this.getTaskID());
   	setTotalDelay(newDelay);	
   	IPVSScheduler.getInstance().scheduleOneShot(this, getTotalDelay());
       }
    protected synchronized void addStream(String roomNID, String streamNID, String sourceAgentJID, Element streamDataDoc, MRLog log, MRRequest request, MRDB db) throws Exception {
	// check whether stream exists in room db
	log.addInfo("RoomGC.addStream streamNID" + streamNID + " roomNID=" + roomNID+" sourceAgentJID= "+sourceAgentJID, MRLog.OK, MRLog.NOTIFY);
	this.request = request;
	//Hashtable<String, String> streams;
	StreamData streams;
	if(!roomGCList.containsKey(roomNID)){	    
	    //streams = new Hashtable<String, String>();
	    streams = new StreamData();
	}  
	else{
	    //streams = roomGCList.get(roomNID);
	    streams = roomGCList.get(roomNID);
	}
	if (RoomDB.getRoomDB().roomExists(roomNID)){
	    if(streamExists(roomNID, streamNID, log, request, db)){
		log.addInfo("RoomGC.addStream room and stream Exists in mediaroom: StreamNID: "+streamNID+":: RoomNID: "+roomNID, MRLog.OK, MRLog.NOTIFY);
		return;
	    }
	}
	 log.addInfo("RoomGC.addStream stream does not exist in mediaroom" + streamNID, MRLog.OK, MRLog.NOTIFY);
	 //streams.put(streamNID, sourceAgentJID);
	 streams.addStreamData(streamNID, streamDataDoc);
	 streams.addSourceAgentJID(streamNID, sourceAgentJID);
	 roomGCList.put(roomNID, streams);
	 
    }

    protected synchronized void deleteStream(String roomNID, String streamNID, MRLog log) {
	if(roomGCList.containsKey(roomNID)){
	    //Hashtable<String, String> streams = roomGCList.get(roomNID);
	    StreamData streams =  roomGCList.get(roomNID);
	    streams.removeStreamData(streamNID);
	    if(streams.size()==0) //no streams related to the room so remove the roomNID from list
		roomGCList.remove(roomNID);
	}
    }
    
    protected synchronized boolean streamExists(String roomNID, String streamNID, MRLog log,MRRequest request, MRDB db) throws Exception {
	return RoomDB.getRoomDB().getRoom(roomNID, request, log, db).objectExists(request, db, log, streamNID);
    }
    private synchronized int getTotalDelay(){
  	return totalDelay;
      }
      private synchronized int setTotalDelay(int delay){
     	return totalDelay+delay;
         }
    @Override
    public void run() {
	//clean the roomGCList 
	log.addInfo("RoomGC.run cleanup called: "+roomGCList.size(), MRLog.OK, MRLog.NOTIFY);
	Enumeration<String> keys = roomGCList.keys();
	while(keys.hasMoreElements()) {	
	    String key = keys.nextElement();
	    //Hashtable<String, String> streams = roomGCList.get(key);
	    StreamData streams =  roomGCList.get(key);
	    Enumeration<String> streamKeys = streams.getStreamData().keys();
	    while(streamKeys.hasMoreElements()) {
		String streamNID = streamKeys.nextElement();
		try {
		    if (RoomDB.getRoomDB().roomExists(key)){
			if(streamExists(key, streamNID, log, request, db)){
			    log.addInfo("RoomGC.run stream exists in mediaroom" + streamNID, MRLog.OK, MRLog.NOTIFY);
			    return;
			}
		    }
		    //release the multicast IP
		    Element streamElement = streams.getStreamDataDoc(streamNID);
		    String sourceNID = MRXMLUtils.getValueXpath(streamElement, ".//sourceNID", "");
	        	String relayNID = MRXMLUtils.getValueXpath(streamElement, ".//relayNID", "");
	        	String multicastIP = MRXMLUtils.getValueXpath(streamElement, ".//multicastIP", "");
	        	AppServerAgentUtils.getServiceConfigManager(serviceAgentFactory).getServiceConfigNetworkResources().releaseMulticastIPAddress(streamNID, sourceNID, relayNID, multicastIP);
		    //stop the stream
		    String actionMessage = StreamAgentUtils.createSourceStreamAgentActionRequestMessage(streamNID, MediaRoomServiceConst.STOP, MediaRoomServiceConst.STOPREQUEST, 
			    null, 0, null, null, null, log);	     
		    log.addInfo("RoomGC.run stream stop request called" + streamNID+" toJID: "+streams.getSourceAgentJID(streamNID)+" multicast IP: "+multicastIP+" ::sourceNID: "+sourceNID+" :: relayNID: "+relayNID, MRLog.OK, MRLog.NOTIFY);
		    //String streamNID = MRXMLUtils.getAttributeValue(streamElement, "NID","");
	        	
		    AppServerAgent.sendAgentRequestForUserRequest(this.request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
			    streams.getSourceAgentJID(streamNID), "StopRequest", MRConst.MEDIA_SERVICE, actionMessage, this.getClass().getName());		    

		} catch (Exception e) {
		    e.printStackTrace();
		}

	    }
	}
	roomGCList.clear();


    }

  private class StreamData{ // in case streamdatadoc is required
	private Hashtable<String, Element> streamData;
	private Hashtable<String, String> sourceAgentData;
	
	StreamData(){
	    sourceAgentData =   new Hashtable<String,String>();
	    streamData = new Hashtable<String, Element>();
	}
	
	void addStreamData(String streamNID, Element streamDataDoc) throws Exception{
	    streamData.put(streamNID, streamDataDoc);
	}
	void addSourceAgentJID(String streamNID, String sourceAgentJID) throws Exception{
	    sourceAgentData.put(streamNID, sourceAgentJID);
	}
	
	Element getStreamDataDoc(String streamNID){
	    return streamData.get(streamNID);
	}
	String getSourceAgentJID(String streamNID){
	    return sourceAgentData.get(streamNID);
	}
	void removeStreamData (String streamNID){
	    streamData.remove(streamNID);
	    sourceAgentData.remove(streamNID);
	}
	int size(){
	    return streamData.size();
	}
	
	Hashtable<String, Element> getStreamData(){	    
	    return streamData;
	}
	/*Hashtable <String, String> getSourceAgentJIDs(){
	    return sourceAgentData;
	}*/
    }
}
