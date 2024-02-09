package com.barco.vng.common;

import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.common.StreamCache;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.utils.FileUtils;
import com.barco.utils.XMLUtils;

public class DeviceDB implements DeviceDBI{


	private String configLoader = "";
	private String statusLoader = "";
	private Document configDocument =null; 
	private Document statusDocument =null;
	private Document apiKeyTableDocument = null;
	private Hashtable<String,Element> portResourceTable = new Hashtable<String,Element>();
	private String deviceResourceNID = null;
	StreamCache streamCache = new StreamCache();
	String ipaddress = "";
	public void addPortResource(String id,Element portResourceElement) {
		portResourceTable.put(id,portResourceElement);
	}
	
	
	public String getDeviceResourceNID() {
		return deviceResourceNID;
	}

	public void setDeviceResourceNID(String deviceResourceNID) {
		this.deviceResourceNID = deviceResourceNID;
	}


	private String deviceXMLNID = null;
	
	
	public String getDeviceXMLNID() {
		return deviceXMLNID;
	}

	public void setDeviceXMLNID(String deviceXMLNID) {
		this.deviceXMLNID = deviceXMLNID;
	}

	public DeviceDB(){
	}
	
	public String getDeviceType() {
		return this.getDeviceConfig().getRootElement().getName();
	}
	/**
	 * Returns the device xml. If device xml not find in local cash it will read from disk. 
	 * @return
	 */
	public Document getDeviceConfig(){
		if(this.configDocument == null) {
			try {
				String strDeviceXML = FileUtils.readFile(this.configLoader);    		
				this.configDocument = XMLUtils.stringToDocument(strDeviceXML);
			} catch (Exception e) {			
				e.printStackTrace();
			}
		}

		return configDocument;
	}


	public Document getDeviceStatus(){
		if(this.statusDocument == null) {
			try {
				String strDeviceXML = FileUtils.readFile(this.statusLoader);    		
				this.statusDocument = XMLUtils.stringToDocument(strDeviceXML);
			} catch (Exception e) {			
				e.printStackTrace();
			}
		}

		return statusDocument;
	}

	public void setDeviceConfig(Document deviceConfigDocument) throws Exception {		
		FileUtils.writeFile(this.configLoader,XMLUtils.documentToString(deviceConfigDocument));
		this.configDocument = deviceConfigDocument;
	}	


	public void setDeviceStatus(Document deviceDoc) throws Exception{
		FileUtils.writeFile(this.statusLoader,XMLUtils.documentToString(deviceDoc));
		this.statusDocument = deviceDoc;
	}
	
	public void resetPortResourceTable(){		
		this.portResourceTable.clear();		
	}


	@Override
	public void setDeviceConfigLoader(String configLoader) {
		this.configLoader = configLoader;
	}


	@Override
	public void setDeviceStatusLoader(String statusLoader) {
		this.statusLoader = statusLoader;
	}
	@Override
	public StreamCache getStreamCache() {
		return this.streamCache;
	}


	@Override
	public String getIPAddress() {
		if(ipaddress.isEmpty())
		 return "127.0.0.1";
		return ipaddress;
	}


	@Override
	public void setIPAddress(String ipaddress) {
		this.ipaddress = ipaddress;
	}


	@Override
	public Element getPortResource(String id) {
		return this.portResourceTable.get(id);
		
	}
	
	public void updateAPIKey(Document deviceLoginResponseDoc) throws Exception {
		Element APIKeyTable = XMLUtils.getElementXpath(deviceLoginResponseDoc, "//ApiKeyTable");
		FileUtils.writeFile(DeviceAdminConst.API_KEY_TABLE_FILE_PATH, XMLUtils.elementToString(APIKeyTable));
	}
	
	public Document getAPIKey() throws Exception {
		if(this.apiKeyTableDocument == null) {
			try {
				String apiKeyTable = FileUtils.readFileWithPath("/var/status/", "APIKeyTable.xml");   		
				this.apiKeyTableDocument = XMLUtils.stringToDocument(apiKeyTable);
			} catch (Exception e) {			
				e.printStackTrace();
			}
		}
		return apiKeyTableDocument;
	}
	
	public String getDeviceIPAddress() throws Exception {
		Document deviceStatusDoc = this.getDeviceStatus();
		String ipAddressXpath  = "//Parameters[Parameter[@name='Status']='True' and Parameter[@name='Primary']='True']/Parameter[@name='IPAddress']";
		String ipAddress = XMLUtils.getValueXpath(deviceStatusDoc, ipAddressXpath, "");  
		if(ipAddress.isEmpty()) {
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTAVAILABLE, "Device IP Address Not Found");
		}
		return ipAddress;
	}
	
	
}
