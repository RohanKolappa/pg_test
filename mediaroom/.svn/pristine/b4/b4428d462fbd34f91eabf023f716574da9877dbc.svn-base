package com.ipvs.device;


import com.ipvs.app.ServiceUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.device.StreamAgents.DeviceConfigStreamAgent;
import com.ipvs.device.StreamAgents.DeviceSourceStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;


public class V2DTxApp extends DeviceApp {
    public static final String V2DTX = "V2D-Tx-Device";

    /**
     * @param args
     */
    public static void main(String[] args) {
        V2DTxApp tx = new V2DTxApp();
        tx.deviceType = AMConst.V2D_TX_DEVICE_OBJECTYPE;
        tx.processCmdLine(args);
        System.exit(1);
    }

    @Override
    public String getDefaultConfigFile() {
        // TODO Auto-generated method stub
        return V2DTX;
    }

    @Override
    public void addDefaultAgents(String loginResponse) {       
        deviceAdminAgent = new DeviceAdminServerAgent(localConfigFile);
        cfgAgent = new DeviceConfigStreamAgent(resourceID, localConfigFile,this.getConfigAgentInitDocument());
        //xauthAgent = new DeviceXAuthServerAgent(proxyQueue);
        srcAgent = new DeviceSourceStreamAgent(localConfigFile, this);

        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.DEVICEADMINSERVERAGENT, deviceAdminAgent);

       // ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.DESTSTREAMAGENT, new DeviceDestStreamAgent());
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.SOURCESTREAMAGENT, srcAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.CONFIGSTREAMAGENT, cfgAgent);

        //ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.XAUTHSERVERAGENT, xauthAgent);
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
