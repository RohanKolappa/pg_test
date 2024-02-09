package com.ipvs.common.service;

import java.io.File;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.device.impl.DeviceAppUtils;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.device.impl.DeviceRequestHandlerI;
import com.ipvs.mediaroomservice.impl.ConfigDeviceUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.resource.handler.SetDeviceConfigRequestHandler;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;
//import com.barco.utils.XMLUtils;

public class DeviceAdminServerAgent implements ServiceAgentI {
	
	
	
	MRLog log=null;
	String localConfig = null;	
    private static final String GET_STREAMING_STATUS_CLI_CMD = "get_streaming_status";
    
    public DeviceAdminServerAgent(String localConfig) {
    	this.localConfig = localConfig;
    }
  
	@Override
	public boolean handleEvent(MRClient client, Document doc, Element event)
			throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public String handleMessage(MRClient client, String fromJID, String message)
			throws Exception {
		
        client.getLog().addInfo("DeviceAdminServerAgent: Received Request from DeviceAdminClientAgent=" + fromJID);

        Document doc = MRXMLUtils.stringToDocument(message);
        String clientCallback = MRXMLUtils.getValueXpath(doc, "//"+MRConst.CLIENT_CALLBACK, "");
        String  requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, null);
        Element deviceAdminResponse=null;
        String responseName=null;
        Element status = MRXMLUtils.newElement("Status");
        Element code = MRXMLUtils.newElement("Code");
        Element description =MRXMLUtils.newElement ("Description");
        MRXMLUtils.addElement(status,code);
        MRXMLUtils.addElement(status,description);
        
        int exceptionCode = MRExceptionCodes.DEVICEADMIN_FAILED;
        String exceptionDesc= "";
        
        if (MRServiceConst.DEVICESTATUSREQUEST.compareToIgnoreCase(requestName) == 0 || "GetDeviceStatusRequest".equals(requestName)) {
        	responseName = StringUtils.replace(requestName, "Request", "Response");
        	ConfigDeviceUtils.updateStatus(log);
        	Document statusDoc = MRXMLUtils.getDocument(new File (ConfigDeviceUtils.DEVICESTATUSFILE));
        	String statusXPath = MRXMLUtils.getValueXpath(doc, "//RequestXPath", "");
        	Element el=null;

        	try {
        		el = MRXMLUtils.getElementXpath(statusDoc,"//StreamingServiceStatus/InstanceList");
        		el.detach();
        	}
        	catch (Exception e) {
        	}
        	try {
        		el = MRXMLUtils.getElementXpath(statusDoc,"//XMPPAgentServiceSourceAgentStatus/Parameters");
        		el.detach();
        	}
        	catch (Exception e) {
        	}

        	Element sourceTable = null;
        	Document sourceStatus = SourceStreamAgent.getStatus(client.getUserJID());
        	Element ssInstanceList=MRXMLUtils.newElement("InstanceList");
		
        	if ((sourceStatus != null) && 
                   (!sourceStatus.getRootElement().elements().isEmpty())) {
        		// Get the Port Instance list Shanker 10/20


        		Element portInstanceList = MRXMLUtils.getElementXpath(sourceStatus, "//PortInstanceList");
        		portInstanceList.detach();
        		portInstanceList.setName("InstanceList");

        		sourceTable = MRXMLUtils.getElementXpath(sourceStatus, "//SourceTable");
        		sourceTable.detach();
        		Element destTable = MRXMLUtils.getElementXpath(sourceStatus, "//DestTable");
        		if(destTable!=null) {
        			destTable.detach();
        		}	


        		Element sourceStreamAgentStatus = MRXMLUtils.getElementXpath(statusDoc, "//XMPPAgentServiceSourceAgentStatus");
        		Element destStreamAgentStatus = MRXMLUtils.getElementXpath(statusDoc, "//XMPPAgentServiceDestinationAgentStatus");
        		
        		if(sourceStreamAgentStatus != null) {
        		     sourceStreamAgentStatus.add(portInstanceList);
        		     sourceStreamAgentStatus.add(sourceTable);
        		     if(destTable!=null) {
        		    	 sourceStreamAgentStatus.add(destTable);
        		     }	 
        		    	 
        		} else if(destStreamAgentStatus != null) {
        			destStreamAgentStatus.add(portInstanceList);
        			destStreamAgentStatus.add(sourceTable);
        			if(destTable!=null) {
        				destStreamAgentStatus.add(destTable);
       		     	}
       		    }
        		
        		String[] cmdArray = { GET_STREAMING_STATUS_CLI_CMD };
        		Document ssDetailsDoc = ServiceAgentUtils.executeCLICommand(cmdArray, log);
        		List<?> parametersList = MRXMLUtils.getListXpath(ssDetailsDoc, "//Parameters");
                Element parametersEL = null;
                String ssInstanceId = null;
                Iterator<?> parametersListIT = parametersList.iterator();
                
                while (parametersListIT.hasNext()) {
                    Element ssElement=null;
                    parametersEL = (Element) parametersListIT.next();
                    ssInstanceId = MRXMLUtils.getValueXpath(parametersEL,
                            "Parameter[@name='Id']", "");
                    ssElement = MRXMLUtils.getElementXpath(sourceStatus,
                            "//DeviceInstanceList/*[SSInstanceID='" +
                            ssInstanceId + "']");
                    if (ssElement != null) {
                        ssElement.detach();
                        ssElement.clearContent();
                    }
                    else {
                        ssElement = MRXMLUtils.newElement("SSInternalInstance");
                        ssElement.addAttribute("NID", ssInstanceId);
                    }
                    parametersEL.detach();
                    ssElement.add(parametersEL);
                    ssInstanceList.add(ssElement);
                }


        	}

        	Element streamingServiceStatus = MRXMLUtils.getElementXpath(statusDoc, "//StreamingServiceStatus");
        	if (ssInstanceList != null && streamingServiceStatus != null) {
        		streamingServiceStatus.add(ssInstanceList);
        	}
        	
        	
        	
        	
        	//System.out.println("==================================== " + MRXMLUtils.documentToString(statusDoc));
        	
        	// Extract required status Shanker a response message  01/15/09
    		Element statusEL = null;
    		try {
                 statusEL = MRXMLUtils.getElementXpath(statusDoc, "//" + ConfigDeviceUtils.DEVICESTATUS + statusXPath);
                 statusEL.detach();
             } catch (Exception e) {
                 e.printStackTrace();
                 statusEL = MRXMLUtils.newElement("Error");
             }
             
             

             Element deviceStatusEL = MRXMLUtils.newElement(ConfigDeviceUtils.DEVICESTATUS);

             if (statusXPath.equals("")) {
                 deviceStatusEL = statusEL;
             } else {
                 deviceStatusEL = MRXMLUtils.newElement(ConfigDeviceUtils.DEVICESTATUS);
                 deviceStatusEL.add(statusEL);
             }

        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");
        	deviceAdminResponse.add(status);
        	deviceAdminResponse.add(deviceStatusEL);        	
        	
        }
        else if (MRServiceConst.DEVICECOMMANDREQUEST.compareToIgnoreCase(requestName)== 0) {
        	responseName = MRServiceConst.DEVICECOMMANDRESPONSE;
            ConfigDeviceUtils.executeDeviceCommand(doc, log);
            deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");
            deviceAdminResponse.add(status);
        }
        else if (MRServiceConst.DEVICECONFIGURATIONREQUEST.compareToIgnoreCase(requestName)== 0 ||
       		MRRequest.getRequestName(SetDeviceConfigRequestHandler.class.getName()).equals(requestName)) {
        	responseName = MRServiceConst.DEVICECONFIGURATIONRESPONSE;
        	   int errorcode = handleSetDeviceConfigRequest(doc);
        	   if (errorcode != 0){
        		   exceptionCode = MRExceptionCodes.CAN_NOT_APPLY_CONFIG;
        		   exceptionDesc = "Can not apply config";
        		   deviceAdminResponse = MRXMLUtils.stringToElement("<error>"+exceptionDesc+"</error>");
        	   } else {
        		   deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");
        		   deviceAdminResponse.add(status);
        	   }
       }
        else if (MRServiceConst.UPGRADEDEVICEREQUEST.compareToIgnoreCase(requestName)== 0) {  
        	responseName = MRServiceConst.UPGRADEDEVICERESPONSE;
        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");        	
        	upgradeDevice(doc);        	  
        	deviceAdminResponse.add(MRXMLUtils.stringToElement("<Success/>"));        	        
        } else if (MRServiceConst.REBOOTDEVICEREQUEST.compareToIgnoreCase(requestName)== 0) {  
        	responseName = MRServiceConst.REBOOTDEVICERESPONSE;
        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");        	
        	reboot();        	  
        	deviceAdminResponse.add(MRXMLUtils.stringToElement("<Success/>"));        	        
        } else if (MRServiceConst.REVERTTODEFAULTDEVICEREQUEST.compareToIgnoreCase(requestName)== 0) {  
        	responseName = MRServiceConst.REVERTTODEFAULTDEVICERESPONSE;
        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");        	
        	revertToDefault(doc);        	  
        	deviceAdminResponse.add(MRXMLUtils.stringToElement("<Success/>"));        	        
        } else if (MRServiceConst.SAVECONFIGDEVICEREQUEST.compareToIgnoreCase(requestName)== 0) {  
        	responseName = MRServiceConst.SAVECONFIGDEVICERESPONSE;
        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");        	
        	saveConfiguration();        	  
        	deviceAdminResponse.add(MRXMLUtils.stringToElement("<Success/>"));        	        
        } else if (MRServiceConst.SHUTDOWNDEVICEREQUEST.compareToIgnoreCase(requestName)== 0) {  
        	responseName = MRServiceConst.SHUTDOWNDEVICERESPONSE;
        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");        	
        	shutdown();        	  
        	deviceAdminResponse.add(MRXMLUtils.stringToElement("<Success/>"));        	        
        }
        else if (MRServiceConst.GETDEVICECONFIGREQUEST.compareToIgnoreCase(requestName)== 0) {  
        	responseName = MRServiceConst.GETDEVICECONFIGRESPONSE;
        	deviceAdminResponse = MRXMLUtils.newElement(responseName + "Data");  
        	String deviceXML = FileUtils.readFile(new File(localConfig));
	    	Document configDoc = MRXMLUtils.stringToDocument(deviceXML);
	    	MRXMLUtils.addElement(deviceAdminResponse,(Element)MRXMLUtils.getElementXpath(configDoc, "//DeviceConfig").detach());        	        	        
        }
        else {
        	String serviceName= MRXMLUtils.getAttributeValueXpath(doc.getRootElement(),MRConst.HEADER_XPATH, MRConst.REQUEST_NAME,"");
        	String handlerName = "com.ipvs."+serviceName+".handler."+requestName+"Handler";
        	try {
        		MRRequest request = MRRequest.createRequest(fromJID, doc);
    			responseName = MRRequest.getResponseName(requestName);    		
    			Class<?> c = Class.forName(handlerName);
    			DeviceRequestHandlerI deviceRequestHandler = (DeviceRequestHandlerI)c.newInstance();
    			if(deviceRequestHandler == null)
        			throw new MRException(MRExceptionCodes.REQUESTNOTFOUND,MRXMLUtils.generateErrorXML("requestName", requestName));
    			Document deviceAdminResponseDoc = deviceRequestHandler.handleDeviceAgentRequest(request,this.localConfig,client);
    			if(deviceAdminResponseDoc != null) {
    				deviceAdminResponse = deviceAdminResponseDoc.getRootElement();        			
    			}
    			
        		}catch (MRException e) {
   			 client.getLog().addInfo(
	            		"DeviceAdminServerAgent: Received Invalid Request from DeviceAdminClientAgent=" 
	            		+ fromJID);
			 deviceAdminResponse = MRXMLUtils.stringToElement("<error>"+e.getData()+"</error>");
			 e.printStackTrace();
		}
		catch (Exception e) {
    			 client.getLog().addInfo(
    	            		"DeviceAdminServerAgent: Received Invalid Request from DeviceAdminClientAgent=" 
    	            		+ fromJID);
    			 deviceAdminResponse = MRXMLUtils.stringToElement("<error>invalid request</error>");
    			 e.printStackTrace();
			}
    		
        	// Drop invalid messages
           
        }
        Document resultdoc = MRXMLUtils.elementToDocument(deviceAdminResponse);
        int requestState = MRRequest.STATE_DONE;
        
        // Error Handling
        
        String logdata = "";
        
        if ("error".equals(resultdoc.getRootElement().getName())){
        		requestState = MRRequest.STATE_EXCEPTION;
                Exception e = new MRException(exceptionCode, exceptionDesc);
        		logdata = e.toString();
        }
        
        
        ServiceAgentUtils.sendAgentResponseMessage(client.getXmpp(),client.getLog(), 
        	null, fromJID, clientCallback, responseName, MRXMLUtils.elementToString(deviceAdminResponse), 
        	"DeviceAdminServerAgent: Sending DeviceAdminResponse from " +
        	MRServiceConst.DEVICEADMINSERVERAGENT + " to " + MRServiceConst.DEVICEADMINCLIENTAGENT,
        	requestState,
            logdata, doc);
        return message;

	}
	
	

	@Override
	public void init(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		// TODO Auto-generated method stub
		this.log=log;
		ServiceAgentUtils.sendAgentStateEvent(userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), MRServiceConst.DEVICEADMINSERVERAGENT, MediaStoreServiceConst.READY, xf.xmpp(userJID), this.log);
	}

	private void upgradeDevice(Document doc) throws Exception
	{
		String upgradeURL = MRXMLUtils.getValueXpath(doc, "//" + SystemServiceConst.URL, "");	
		if (upgradeURL.isEmpty() || upgradeURL == null)
			throw new MRException(SessionServiceExceptionCodes.UPGRADE_URL_MISSING, "Upgrade URL missing");
		
		String[] systemCmd = {"upgrade_device", "-url", upgradeURL};		
		MediaRoomServiceUtils.executeCLICommand(systemCmd);
	}

	@Override
	public void reset() throws Exception {
		// TODO Auto-generated method stub
	}
	
	private void reboot() throws Exception{		
		String[] command = {"reboot_system"};
		MediaRoomServiceUtils.executeCLICommand(command);
	}


	private void revertToDefault(Document doc) throws Exception{
		String keepMedia = MRXMLUtils.getAttributeValueXpath(doc, "//RevertTodefaultDevice", "keepMedia", "");
		String keepNetworkSettings = MRXMLUtils.getAttributeValueXpath(doc, "//RevertTodefaultDevice", "keepNetworkSettings", "");
		boolean deleteMedia = Boolean.getBoolean(keepMedia);
		boolean revertNetworkConfig = Boolean.getBoolean(keepNetworkSettings);
		String revertNetWorkCofigStr = MediaRoomServiceConst.NO;
		if(revertNetworkConfig) {
			revertNetWorkCofigStr = MediaRoomServiceConst.YES;
		}
		
		String deleteMediaStr = MediaRoomServiceConst.NO;
		if(deleteMedia) {
			deleteMediaStr = MediaRoomServiceConst.YES;
		}
		
		String[]  command = {"revert_config", "-reset_network" , revertNetWorkCofigStr , "-delete_media" ,deleteMediaStr};
		MediaRoomServiceUtils.executeCLICommand(command);
	}


	private void saveConfiguration() throws Exception {
		String[] command = {"save_config"};
		MediaRoomServiceUtils.executeCLICommand(command);
	}
	
	private void shutdown() throws Exception{		
		String[] command = {"shutdown_system"};
		MediaRoomServiceUtils.executeCLICommand(command);
	}
	
	public int handleSetDeviceConfigRequest(Document doc) throws Exception {
		String requestName = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "SetDeviceConfigRequest");
		
		if (log == null) {
			log = MRLog.getInstance(MRLog.DAEMON);
		}

		Document docModify = ConfigDeviceUtils.findDirtyChildren(doc, log);
		List<?> list = MRXMLUtils.getList(docModify.getRootElement());
		if ((list == null) || (list.size() == 0)) {
			return MRExceptionCodes.CAN_NOT_APPLY_CONFIG;
		}

		Document resultDoc = ConfigDeviceUtils.handleConfig(docModify, DeviceConst.DEVICE_CONFIG_HANDLER_XML, log);
		// verify the command result if result has error then return failure response
		if (MRXMLUtils.getElementXpath(resultDoc, "//Result/*[.='" + MRRequest.STATE_EXCEPTION + "']") != null) {
			return MRExceptionCodes.CAN_NOT_APPLY_CONFIG;
		} else {
			String doNotSaveToFlash = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(doc, ".//" + requestName + "Data"),
					SessionServiceResourceConst.DO_NOT_SAVE_TO_FLASH_ATTRIBUTE, "");
			if (!Utils.getBooleanValue(doNotSaveToFlash, Boolean.FALSE)) {
						DeviceAppUtils.saveToFlash(log);
			}
			return 0;
		}

	}
	
	public String handleGetDeviceRequest(String userJID) throws Exception {

		String state = MRConst.UN_REGISTERED;
		if(userJID != null){
			state = MRConst.REGISTERED;
		}else{
			userJID ="";
		}
		
		Document deviceConfigXML = MRXMLUtils.getDocument(new File(this.localConfig));
		Document statusXML = MRXMLUtils.getDocument(new File(ConfigDeviceUtils.DEVICESTATUSFILE));

		String configDocument = MRXMLUtils.getElementXpath(deviceConfigXML, "//DeviceConfig").asXML();
		String deviceStatus = MRXMLUtils.getElementXpath(statusXML, "//DeviceStatus").asXML();
		String deviceType = deviceConfigXML.getRootElement().getName();
		String deviceTitle = MRXMLUtils.getValueXpath(deviceConfigXML, "//XMPPAgentServiceConfig/DeviceName", "");
		String password = MRXMLUtils.getValueXpath(deviceConfigXML, "//XMPPAgentServiceConfig/DevicePassword", "");
		String responseXML = "<Device title='" + deviceTitle + "' type='" + deviceType + "' state='" + state + "' password='" + password + "' ownerUserJID='" + userJID + "'>"
				+ "<Groups />" + "<DevicePrivateData /><TemplateNID />" + "" + "<PortList />" + configDocument + deviceStatus + "<DeviceList />" + "</Device>";

		return responseXML;

	}
}
