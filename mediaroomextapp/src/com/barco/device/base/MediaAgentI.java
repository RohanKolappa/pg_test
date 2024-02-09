package com.barco.device.base;

import org.dom4j.Document;
import org.dom4j.Element;

public interface MediaAgentI  extends AgentI{

	// When acting as a Source
	public abstract Element startStream(DeviceI device, String requestNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc)
    throws Exception ;
	public abstract Element stopStream(String streamNID) throws Exception;
	
	// When acting as a Destination
	public abstract void addDest(String streamNID, String sourceAgentJID) throws Exception;
	public abstract String removeDest(String streamNID) throws Exception;
	

}
