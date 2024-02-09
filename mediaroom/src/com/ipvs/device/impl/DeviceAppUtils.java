package com.ipvs.device.impl;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.systemservice.impl.ServiceVersionManager;
import com.ipvs.utils.Utils;


// We do not store the doc since it can get to be huge
// Instead we store the info used to retrieve it from the db
public class DeviceAppUtils {
    public static void setServiceDomain(String domainRequest, ServiceClient client,MRLog log)
        throws Exception {
        Document domainRequestDoc = MRXMLUtils.stringToDocument(domainRequest);
        String serviceVersion = ServiceVersionManager.getServiceVersion(log);
        Element serviceVersionEl = MRXMLUtils.getElementXpath(domainRequestDoc, "//" + MRConst.SERVICE_VERSION);

        if (serviceVersionEl == null) {
            serviceVersionEl = MRXMLUtils.newElement(MRConst.SERVICE_VERSION);
            MRXMLUtils.addElement(domainRequestDoc.getRootElement(), serviceVersionEl);
        }

        MRXMLUtils.setValue(serviceVersionEl, serviceVersion);
        
        //update software version
        String softwareVersion = getSoftwareVersion(log);
        Element softwareVersionEl = MRXMLUtils.getElementXpath(domainRequestDoc, "//" + MRConst.SOFTWARE_VERSION);

        if (softwareVersionEl == null) {
        	softwareVersionEl = MRXMLUtils.newElement(MRConst.SOFTWARE_VERSION);
            MRXMLUtils.addElement(domainRequestDoc.getRootElement(), softwareVersionEl);
        }

        MRXMLUtils.setValue(softwareVersionEl, softwareVersion);
        
        
        domainRequest = MRXMLUtils.documentToStringOmitDeclaration(domainRequestDoc);

        String response = client.setServiceDomain("*", domainRequest);
        Document responseDoc = null;
        responseDoc = MRXMLUtils.getDocument(response);

        Element responseEl = MRXMLUtils.getElementXpath(responseDoc, "//Response");
       
        if (responseEl == null) {
            //waiting for  Set Service Domain response from appserver
            return;
        } else {
            //set service domain request failed. check  the response
            if (Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MRRequest.STATE_EXCEPTION) == MRRequest.STATE_EXCEPTION) {
                int code = Utils.getIntValue(MRXMLUtils.getValueXpath(responseDoc, "//Code", ""), MRExceptionCodes.SET_SERVICE_DOMAIN_FAILED);
                String description = MRXMLUtils.getValueXpath(responseDoc, "//Description", "");
                throw new MRException(code, description);
            }
        }
    }

    public static int getResponseState(String response)
        throws Exception {
        int state = 0;
        Document responseDoc = null;
        try {
			responseDoc = MRXMLUtils.getDocument(response);
            state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, "0"));
		} catch (Exception e) {
			return 0;
		}
   
        return state;
    }

    public static String login(String loginRequest, String linkLocalLoginRequest,ServiceClient client, MRLog log)
        throws Exception {
        int state = 0;
        log.addInfo("Login  started  ");
        if(loginRequest!=null) {
	        Document loginRequestDoc = MRXMLUtils.stringToDocument(loginRequest);
	        String macAddress = getMacAddress(log);
	        Element loginReqiestDataEl = MRXMLUtils.getElementXpath(loginRequestDoc, "//" + MRConst.LOGIN_REQUEST_DATA);
	
	        if (loginReqiestDataEl != null) {            
	            MRXMLUtils.addElement(loginReqiestDataEl, MRConst.CLIENT_KEY,macAddress);
	            loginRequest = MRXMLUtils.documentToString(loginRequestDoc);
	        }        
        }
        String response = client.login(loginRequest,linkLocalLoginRequest);
        state = DeviceAppUtils.getResponseState(response);
        System.out.println("response:="+response);
        if (state == 0) {
            throw new Exception("Login Failed");
        }

        log.addInfo("Login  complete :" + System.currentTimeMillis());

        return response;
    }

    public static void handleNotification(Element notificationXMLEL, String notificationType, String eventSource, String type, String userJID, MRLog log, XMPPI xmpp, MRDB db)
        throws MRException, Exception {
        // System.out.println("DeviceAppUtils:handleNotification SSEvent notificationDoc=" + MRXMLUtils.elementToString(notificationXMLEL));

        Document notificationHandlerDoc = MRXMLUtils.loadXMLResource(DeviceConst.NOTIFICATIONHANDLER, log);
        String noticationTypeXpath = DeviceConst.NOTIFICATION_TYPE + " ='" + notificationType + "'";
        String eventSourceXpath = DeviceConst.EVENT_SOURCE_XPATH + "='" + eventSource + "'";
        String typeXpath = DeviceConst.EVENT_TYPE_XPATH + "='" + type + "'";
        String notificationXpath = "//" + DeviceConst.NOTIFICATION + "[" + noticationTypeXpath + " and " + eventSourceXpath + " and " + typeXpath + "]";

        List<?> notificationElList = MRXMLUtils.getListXpath(notificationHandlerDoc, notificationXpath);
        if (notificationElList == null) {
            return;
        }
        
        Element notifyEl = null;
        for (Object o : notificationElList) {
        	Element notificationEl = (Element) o;
	        //apply data xpath         	
			List<String> dataXPathList = MRXMLUtils.getValuesXpath(notificationEl,DeviceConst.DATA_XPATH_LIST_XPATH);		    
			if (dataXPathList != null && !dataXPathList.isEmpty()) {				
				Document notificationXMLDoc = MRXMLUtils.elementToDocument(notificationXMLEL);				
				for (String xpath : dataXPathList) {
					if(MRXMLUtils.applyXPath(notificationXMLDoc, xpath)) {									
						notifyEl = notificationEl;
						break;
					} 
				}								
			} else {
				notifyEl = notificationEl;
			}
			if(notifyEl != null) {
				break;
			}
        }
        
        if (notifyEl == null) {
        	return;
        }
        
		String mediaRoomEventType = MRXMLUtils.getValueXpath(notifyEl, ".//" + DeviceConst.MEDIA_ROOM_EVENT, "");
        String mediaRoomEventXMLPath = MRXMLUtils.getValueXpath(notifyEl, ".//" + DeviceConst.MEDIA_ROOM_EVENT_XML_PATH, "");
        List<String> serviceList = MRXMLUtils.getValuesXpath(notifyEl, ".//" + DeviceConst.SERVICE_XPATH);
        if (mediaRoomEventType.isEmpty() || mediaRoomEventXMLPath.isEmpty() || (serviceList == null) || serviceList.isEmpty()) {
            return;
        }

        Document mediaRoomEventDoc = MRXMLUtils.loadXMLResource(mediaRoomEventXMLPath, log);

        if (mediaRoomEventDoc == null) {
            return;
        }

        MRXMLUtils.setValueXpath(mediaRoomEventDoc, "//" + DeviceConst.ACTION, mediaRoomEventType);
        MRXMLUtils.addElement(MRXMLUtils.getElementXpath(mediaRoomEventDoc, "//" + mediaRoomEventType + DeviceConst.ACTION_DATA), (Element) notificationXMLEL.clone());

        for (String serviceName : serviceList) {	
            String info = "DeviceApp:=" + userJID + " > Request action=" + mediaRoomEventType + " to " + ServiceAgentUtils.getAgentShortName(serviceName) + " event=" + eventSource;           
            ServiceAgentUtils.sendAgentEvent(xmpp, log, null, "", userJID, serviceName, MRLog.OK, userJID, xmpp.getCurrentTimeMillis(), mediaRoomEventType, mediaRoomEventDoc.getRootElement(), "", info);
        }
    }
    
    public static void handleTarNotification(Element notificationXMLEL, String notificationType, String eventSource, String type, String userJID, MRLog log, XMPPI xmpp, MRDB db) throws MRException, Exception {
        log.addInfo("DeviceAppUtils:handleTarNotification TarEvent notificationDoc:" + MRXMLUtils.elementToString(notificationXMLEL), MRLog.INFO, MRLog.NOTIFY);
        Document notificationHandlerDoc = MRXMLUtils.loadXMLResource(DeviceConst.NOTIFICATIONHANDLER, log);
        String noticationTypeXpath = DeviceConst.NOTIFICATION_TYPE + " ='" + notificationType + "'";
        String eventSourceXpath = DeviceConst.EVENT_SOURCE_XPATH + "='" + eventSource + "'";
        String typeXpath = DeviceConst.EVENT_TYPE_XPATH + "='" + type + "'";
        String notificationXpath = "//" + DeviceConst.NOTIFICATION + "[" + noticationTypeXpath + " and " + eventSourceXpath + " and " + typeXpath + "]";

        Document notificationDoc = MRXMLUtils.getDocumentXpath(notificationHandlerDoc, notificationXpath);

        if (notificationDoc == null) {
            return;
        }

        String mediaRoomEventType = MRXMLUtils.getValueXpath(notificationDoc, "//" + DeviceConst.MEDIA_ROOM_EVENT, "");
        String mediaRoomEventXMLPath = MRXMLUtils.getValueXpath(notificationDoc, "//" + DeviceConst.MEDIA_ROOM_EVENT_XML_PATH, "");
        List<String> agentList = MRXMLUtils.getValuesXpath(notificationDoc.getRootElement(), "//" + DeviceConst.SERVICE_XPATH);

        if (mediaRoomEventType.isEmpty() || mediaRoomEventXMLPath.isEmpty() || (agentList == null) || agentList.isEmpty()) {
            return;
        }

        Document mediaRoomEventDoc = MRXMLUtils.loadXMLResource(mediaRoomEventXMLPath, log);
        if (mediaRoomEventDoc == null) {
            return;
        }

        MRXMLUtils.setValueXpath(mediaRoomEventDoc, "//" + DeviceConst.ACTION, mediaRoomEventType);
        MRXMLUtils.addElement(MRXMLUtils.getElementXpath(mediaRoomEventDoc, "//" + mediaRoomEventType + DeviceConst.ACTION_DATA), (Element) notificationXMLEL.clone());

        for (String agentName : agentList) {
            String info = "DeviceApp:=" + userJID + " > Request action=" + mediaRoomEventType + " to " + ServiceAgentUtils.getAgentShortName(agentName) + " event=" + eventSource;
            //System.out.println("agentName=" + agentName +" info="+info);
            ServiceAgentUtils.sendAgentEvent(xmpp, log, null, "", userJID, agentName, MRLog.OK, userJID, xmpp.getCurrentTimeMillis(), mediaRoomEventType, mediaRoomEventDoc.getRootElement(), "", info);
        }
    }

    public static void publishStatus(String localConfigFile, MRLog log) {
    
    	String cmd = null;
    	Element el = null;
     	String cliargument = null;
    	String cliparameter = null;

    	try {

    		Hashtable<Element,List<?>> listOfCommandStoBeExecuted = getListOfCommandsToBeExevuted(localConfigFile, log);

    		for(Element notificationEl : listOfCommandStoBeExecuted.keySet()) {
    			cmd = MRXMLUtils.getValue(notificationEl, "cmd", "none");
    			cliargument = MRXMLUtils.getAttributeValue(notificationEl, "parent", 
    					"cliargument", null);
    			List<?> lc = listOfCommandStoBeExecuted.get(notificationEl);
    			for (int j = 0; j < lc.size(); j++) {
    				ArrayList<String> cmdArray = new ArrayList<String>();
    				cmdArray.add(cmd);
    				cmdArray.add("-publish");
    				if (cliargument != null) {
    					el = (Element) lc.get(j);
    					cliparameter = el.getStringValue();
    					cmdArray.add("-" + cliargument);
    					cmdArray.add(cliparameter);
    				}
    				ServiceAgentUtils.executeCLICommand(cmdArray.toArray(new String[cmdArray.size()]), log);
    			}				
    		}
    	} catch(Exception exp){
    		exp.printStackTrace();
    	}

    }
    
    
    
    public static Hashtable<Element,List<?>>  getListOfCommandsToBeExevuted(String localConfigFile, MRLog log) {
    	Document dochandler = null;
    	Document docConfig = null;

    	Element el=null;
    	String locator=null;
    	String trigger = null;
    	String cliargument = null;
    	Hashtable<Element,List<?>> listOfCommandsToBeExecuted = new Hashtable<Element, List<?>>();
    	
    	try {
    		dochandler = MRXMLUtils.loadXMLResource(DeviceConst.STATUSHANDLER, log);
    		docConfig = MRXMLUtils.loadXMLFile(localConfigFile, log);
    		List<?> lh = MRXMLUtils.getListXpath(dochandler, "//handler[publish='true']");
    		if ((lh != null) && !lh.isEmpty()) {
    			for (int i = 0; i < lh.size(); i++) {
    				el = (Element)lh.get(i);
    			    trigger = MRXMLUtils.getValue(el, "triggername", "none");
    				locator = MRXMLUtils.getAttributeValue(el, "parent", 
    						"relativeXpath", null);    						
    				cliargument = MRXMLUtils.getAttributeValue(el, "parent", 
    						"cliargument", null);
    				List<?> lc = null;
    				if (cliargument != null) {
    					lc = MRXMLUtils.getListXpath(docConfig, 
    							"//" + trigger + "/" + locator);
    				}
    				else {
    					lc = MRXMLUtils.getListXpath(docConfig, 
    							"//" + trigger);
    				}
    				
    				if(lc != null && !lc.isEmpty()){
    					listOfCommandsToBeExecuted.put(el, lc);
    				}    				
    			}

    		}

    	} catch (Exception e) {
    		e.printStackTrace();
    	}
    	
    	return listOfCommandsToBeExecuted;

    }

	//get mac address of device
	public static String getMacAddress(MRLog log) throws MRException,Exception {
		String cmd[] = {"get_network_ports_status"};
		Document networkStatusDoc = ServiceAgentUtils.executeCLICommand(cmd, log);
		String macAddressXpath  = "//Parameters[Parameter[@name='Status']='True']/Parameter[@name='HardwareAddress']";
		String macAddress = MRXMLUtils.getValueXpath(networkStatusDoc, macAddressXpath, "");  
		if(macAddress.isEmpty()) {
			throw new MRException(MRExceptionCodes.DEVICE_MAC_ADDRESS_NOT_FOUND, "Device Mac Address Not Found");
		}
		return  macAddress;
	}
	
	//get  IPaddress of device
	public static String getIPAddress(MRLog log) throws MRException,Exception {
		String cmd[] = {"get_network_ports_status"};
		Document networkStatusDoc = ServiceAgentUtils.executeCLICommand(cmd, log);
		String IPAddressXpath  = "//Parameters[Parameter[@name='Status']='True' and Parameter[@name='Primary']='True']/Parameter[@name='IPAddress']";
		String IPAddress = MRXMLUtils.getValueXpath(networkStatusDoc, IPAddressXpath, "");  
		if(IPAddress.isEmpty()) {
			throw new MRException(MRExceptionCodes.DEVICE_IPADDRESS_NOT_FOUND, "Device IP Address Not Found");
		}
		return IPAddress;
	}
	
	//get software version of device
	public static String getSoftwareVersion(MRLog log) throws MRException,Exception {
		String cmd[] = {"get_software_image_status"};
		Document softwareStatusDoc = ServiceAgentUtils.executeCLICommand(cmd, log);
		String getSoftwareXpath  = "//Parameters/Parameter[@name='Version']";
		return  MRXMLUtils.getValueXpath(softwareStatusDoc, getSoftwareXpath, "");  
	}
	
	
	//save_config
	public static void saveToFlash(MRLog log) throws MRException,Exception {
		log.addInfo("Save configuration");
		String[] systemCmd = {"save_config"};
		MediaRoomServiceUtils.executeSystemCommand(systemCmd);
	}
}
