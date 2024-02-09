package com.barco.device.common;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.Map.Entry;

import org.dom4j.Element;

import com.barco.utils.XMLUtils;


public class StreamCache {
	Hashtable<String, StreamEntry> streamList = new Hashtable<String, StreamEntry>();
	public void addStreamEntry(String streamNID, StreamEntry streamEntry) throws Exception {
		this.streamList.put(streamNID, streamEntry);
	}
	
	public  synchronized StreamEntry getStreamEntry(String streamNID) {
		return this.streamList.get(streamNID);
	}
	
	public synchronized StreamEntry deleteStreamEntry(String streamNID) {
		StreamEntry se = this.streamList.get(streamNID);
		this.streamList.remove(streamNID);
		return se;
	}
	
	public  synchronized StreamEntry getStreamEntryUsingFileUUID(String uuid) throws Exception {
		for(String streamNID : streamList.keySet()) {
			StreamEntry streamEntry = streamList.get(streamNID);
			String destFileUUID = XMLUtils.getValueXpath(streamEntry.streamDataDoc, "//destFileUUID", "");
			if(destFileUUID.equals(uuid)){
				return streamEntry;
			}
		}
		return null;
	}
	
	public ArrayList<String> getStreamNIDListByRoomNID(String roomNID) {
		ArrayList<String> list = new ArrayList<String>();
		Enumeration<String> keys = streamList.keys();
		while(keys.hasMoreElements()) {
			StreamEntry entry = streamList.get(keys.nextElement());
			if(entry.roomNID.equals(roomNID))
				list.add(entry.streamNID);
		}
		return list;
	}
	
	public StreamEntry getStreamByDestination(String destPortElementID,String destNID) throws Exception {
		for (Entry<String, StreamEntry> entry : streamList.entrySet()) {
			StreamEntry se = entry.getValue();
				String streamDestNID = XMLUtils.getValueXpath(se.streamDataDoc, "//destNID","");
				if(streamDestNID.equals(destNID) && destPortElementID.equals(se.destPortElementID)){
					return se;			
				}				
		}
		return null;
	}

	public StreamEntry getStreamByDestPortElementID(String destPortElementID) {
		Enumeration<String> keys = streamList.keys();
		while(keys.hasMoreElements()) {
			StreamEntry entry = streamList.get(keys.nextElement());
			if(entry.destPortElementID.equals(destPortElementID))
				return entry;
		}
		return null;
	}
	
	
	public List<StreamEntry> getStreamByDirPath(String dirPath) throws Exception {
		Enumeration<String> keys = streamList.keys();
		List<StreamEntry> listOfStreamEntry = new ArrayList<StreamEntry>();
		while(keys.hasMoreElements()) {
			StreamEntry entry = streamList.get(keys.nextElement());
			String streamDirPath = XMLUtils.getValueXpath(entry.streamDataDoc, "//destFileParentDirPath", "");
			if(streamDirPath.equals(dirPath))
				listOfStreamEntry.add(entry);
		}
		return listOfStreamEntry;
	}
	
	public List<StreamEntry> getStreamBySourcePortNID(String sourceNID){
		List<StreamEntry> out = new ArrayList<StreamEntry>();
		for (Entry<String, StreamEntry> entry : streamList.entrySet()) {
			StreamEntry se = entry.getValue();
			try{
				Element nidEl = XMLUtils.getElementXpath(se.mediasourceDataDoc, "//sourcePortNID");
				if(nidEl != null && sourceNID.equals(nidEl.getText())){
					out.add(se);				
				}				
			}catch (Exception e){
				//don't care about exception
			}
		}
		return out;
	}

	public ArrayList<String> getStreamNIDListBySourceAgentJID(String sourceUserJID) {
		ArrayList<String> list = new ArrayList<String>();
		Enumeration<String> keys = streamList.keys();
		while(keys.hasMoreElements()) {
			StreamEntry entry = streamList.get(keys.nextElement());
			if(entry.sourceAgentJID.equals(sourceUserJID))
				list.add(entry.streamNID);
		}
		return list;
	}
	
	public Hashtable<String, StreamEntry> getStreamList(){
		return this.streamList;
	}
	
	public void resetCache() {
		this.streamList.clear();
	}
}

