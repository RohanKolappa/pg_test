package com.barco.device.common;

import org.dom4j.Document;
import org.dom4j.Element;

public interface DeviceDBI {
	
	public String getDeviceType();	
	public Document getDeviceConfig();	
	public Document getDeviceStatus();	
	
	//set loaders information
	public void setDeviceConfigLoader(String configLoader);
	public void setDeviceStatusLoader(String statusLoader);
	
    public void setDeviceConfig(Document deviceConfigDocument) throws Exception ;
	public void setDeviceStatus(Document deviceDoc) throws Exception;
	
	public void addPortResource(String id,Element portResourceElement) ;
	public void resetPortResourceTable();
	public Element getPortResource(String id);
	
	public String getDeviceResourceNID();
	public void setDeviceResourceNID(String deviceResourceNID) ;
	public void setDeviceXMLNID(String deviceXMLNID);	
	//TODO:this is temporary till we remove device xml from database
	public String getDeviceXMLNID();
	public abstract StreamCache getStreamCache();
	public String getIPAddress();
	public void setIPAddress(String ipaddress);
	public abstract void updateAPIKey(Document deviceLoginResponseDoc) throws Exception;
	public abstract Document getAPIKey() throws Exception;
	public String getDeviceIPAddress() throws Exception;
	
}
