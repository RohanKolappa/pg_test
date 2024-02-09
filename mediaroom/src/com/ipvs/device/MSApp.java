package com.ipvs.device;

import org.dom4j.Document;

import com.ipvs.app.ServiceUtils;
import com.ipvs.common.AgentInitUtils;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.device.StreamAgents.DeviceConfigStreamAgent;
import com.ipvs.device.http.server.DeviceHttpServerAgent;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.systemservice.handler.DownloadDBRequestHandler;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.FileUtils;


public class MSApp extends DeviceApp {
    public static final String MS = "MS-Device";
    public static final String MSDEVICEINFO= "/etc/config/hw_profile_ms.txt";

    public static void main(String[] args) {
        MSApp ms = new MSApp();        
        ms.processCmdLine(args);
        System.exit(1);
        
    }

    @Override
    public String getDefaultConfigFile() {
    	String deviceInfo = null;
   		try {
			deviceInfo = FileUtils.readFile(MSDEVICEINFO);
			deviceInfo = deviceInfo.substring(0, deviceInfo.length()-1);
		} catch (Exception e) {
			log.addInfo("Not a valid MS");
			deviceInfo=MS;
		}
        return deviceInfo;
    }

    @Override
    public void addDefaultAgents(String loginResponse) {       
        //deviceAdminAgent = new DeviceAdminServerAgent(localConfigFile);
        cfgAgent = new DeviceConfigStreamAgent(resourceID, localConfigFile,this.getConfigAgentInitDocument());
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.DEVICEADMINSERVERAGENT, deviceAdminAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.CONFIGSTREAMAGENT, cfgAgent);
        // Added this for nightly to pass
		MediaStoreServiceConst.init();
    }

    @Override
	public boolean runSSNotificationListener() {
		// TODO Auto-generated method stub
		return false;
	}
    
    //returns the agent init task to be done for MS devices
    protected Document getConfigAgentInitDocument() {
    	Document agentInitTaskDoc = null;
    	try {
    		agentInitTaskDoc = super.getConfigAgentInitDocument();
    		if(HAUtils.isHAServiceEnabled() && HAUtils.isServerInStandby()) {
            	String requestName = MRRequest.getRequestName(DownloadDBRequestHandler.class.getName());
            	AgentInitUtils.addAgentTask(agentInitTaskDoc, requestName, AgentInitUtils.AGENT_STATE_PENDING, requestName, requestName);
            }
    	}  catch (Exception e) {
    		e.printStackTrace();
    	}
    	return agentInitTaskDoc;
    }

    @Override
	public void startHttpServer() {
		httpServerAgent = new DeviceHttpServerAgent(DeviceConst.HTTP_SERVER_PORT, log);
		deviceAdminAgent = new DeviceAdminServerAgent(localConfigFile);
		httpServerAgent.startServer(deviceAdminAgent);
	}

	@Override
	public void stopHttpServer() {
		try {
			httpServerAgent.stopServer();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}    
}