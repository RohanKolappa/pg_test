package com.ipvs.device;


import org.dom4j.Document;

import com.ipvs.app.ServiceUtils;
import com.ipvs.assetadminservice.device.handler.SetServiceClientConfigRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.AgentInitUtils;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.device.StreamAgents.DeviceConfigStreamAgent;
import com.ipvs.device.StreamAgents.DeviceDestStreamAgent;
import com.ipvs.device.StreamAgents.DeviceSourceStreamAgent;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.utils.FileUtils;


public class V2DXP200App extends DeviceApp {
    public static final String V2DTX = "V2D-XP200-Device";
    public static final String XP200DEVICEINFO = "/etc/config/hw_profile_v2dxp200.txt";

    /**
     * @param args
     */
    public static void main(String[] args) {
        V2DXP200App xp = new V2DXP200App();
        xp.deviceType = AMConst.V2D_XP200_DEVICE_OBJECTYPE;
        xp.processCmdLine(args);
        System.exit(1);
    }

    @Override
    public String getDefaultConfigFile() {
       	String deviceInfo = V2DTX;
   		try {
			deviceInfo = FileUtils.readFile(XP200DEVICEINFO);
			deviceInfo = deviceInfo.substring(0, deviceInfo.length()-1);
		} catch (Exception e) {
			log.addInfo("Not a valid XP200 device");
			deviceInfo = V2DTX;
		}
        return deviceInfo;
    }

    @Override
    public void addDefaultAgents(String loginResponse) {        
        deviceAdminAgent = new DeviceAdminServerAgent(localConfigFile);
        cfgAgent = new DeviceConfigStreamAgent(resourceID, localConfigFile,this.getConfigAgentInitDocument());
        //xauthAgent = new DeviceXAuthServerAgent(proxyQueue);
        srcAgent = new DeviceSourceStreamAgent(localConfigFile, this);

        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.DEVICEADMINSERVERAGENT, deviceAdminAgent);

        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.DESTSTREAMAGENT, new DeviceDestStreamAgent());
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.SOURCESTREAMAGENT, srcAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.CONFIGSTREAMAGENT, cfgAgent);

        //ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.XAUTHSERVERAGENT, xauthAgent);
    }

    @Override
	public boolean runSSNotificationListener() {
		// TODO Auto-generated method stub
		return true;
	}
    
    //returns the agent init task to be done for xp devices 
    protected Document getConfigAgentInitDocument() {
    	Document agentInitTaskDoc = null;
    	try {
    		agentInitTaskDoc = super.getConfigAgentInitDocument();
    		AgentInitUtils.addAgentTask(agentInitTaskDoc, DeviceConst.INIT_PORT_TYPE_UPDATE_TASK_NAME, AgentInitUtils.AGENT_STATE_PENDING, 
    				MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName()), DeviceConst.INIT_PORT_TYPE_UPDATE_TASK_NAME);
    	}  catch (Exception e) {
    		e.printStackTrace();
    	}
    	return agentInitTaskDoc;
    }

	@Override
	public void startHttpServer() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stopHttpServer() {
		// TODO Auto-generated method stub
		
	}

}
