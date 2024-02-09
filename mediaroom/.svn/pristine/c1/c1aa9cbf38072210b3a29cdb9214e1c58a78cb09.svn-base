package com.ipvs.device;

import com.ipvs.app.ServiceUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.device.StreamAgents.DeviceConfigStreamAgent;
import com.ipvs.device.StreamAgents.DeviceDestStreamAgent;
import com.ipvs.device.StreamAgents.DeviceSourceStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;


public class V2DRxApp extends DeviceApp {
    public static final String V2DRX = "V2D-Rx-Device";

    /**
     * @param args
     */
    public static void main(String[] args) {
        V2DRxApp rx = new V2DRxApp();
        rx.deviceType = AMConst.V2D_RX_DEVICE_OBJECTYPE;
        rx.processCmdLine(args);
        System.exit(1);
    }

    @Override
    public String getDefaultConfigFile() {
        // TODO Auto-generated method stub
        return V2DRX;
    }

    @Override
    public void addDefaultAgents(String loginResponse) {       
        deviceAdminAgent = new DeviceAdminServerAgent(localConfigFile);
        cfgAgent = new DeviceConfigStreamAgent(resourceID, localConfigFile,this.getConfigAgentInitDocument());
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.DEVICEADMINSERVERAGENT, deviceAdminAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.DESTSTREAMAGENT, new DeviceDestStreamAgent());
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.SOURCESTREAMAGENT, new DeviceSourceStreamAgent(localConfigFile, this));
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.CONFIGSTREAMAGENT, cfgAgent);

    }

    @Override
	public boolean runSSNotificationListener() {
		// TODO Auto-generated method stub
		return true;
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
