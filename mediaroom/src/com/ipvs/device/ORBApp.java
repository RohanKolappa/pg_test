package com.ipvs.device;


import java.util.ArrayList;

import org.dom4j.Document;

import com.ipvs.app.ServiceUtils;
import com.ipvs.assetadminservice.device.handler.SetServiceClientConfigRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.AgentInitUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.device.StreamAgents.DeviceConfigStreamAgent;
import com.ipvs.device.StreamAgents.DeviceDestStreamAgent;
import com.ipvs.device.StreamAgents.DeviceSourceStreamAgent;
import com.ipvs.device.impl.DeviceAgentUtils;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.utils.FileUtils;


public class ORBApp extends DeviceApp {
    public static final String ORB = "ORB-500-Device";
    public static final String ORBDEVICEINFO = "/etc/config/hw_profile_orb.txt";

    /**
     * @param args
     */
    public static void main(String[] args) {
        ORBApp orb = new ORBApp();
        orb.deviceType = AMConst.ORB_500_DEVICE_OBJECTTYPE;
        orb.processCmdLine(args);
        System.exit(1);
    }

    @Override
    public String getDefaultConfigFile() {
       	String deviceInfo = ORB;
   		try {
			deviceInfo = FileUtils.readFile(ORBDEVICEINFO);
			deviceInfo = deviceInfo.substring(0, deviceInfo.length()-1);
		} catch (Exception e) {
			log.addInfo("Not a valid ORB device");
			deviceInfo = ORB;
		}
        return deviceInfo;
    }

    @Override
    public void addDefaultAgents(String loginResponse) {        
        deviceAdminAgent = new DeviceAdminServerAgent(localConfigFile);
        cfgAgent = new DeviceConfigStreamAgent(resourceID, localConfigFile, this.getConfigAgentInitDocument());
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
    
    public String setTXConnectionParameters(String srcPortElementID, Document streamDoc) {
    	String ioport = srcPortElementID;
    	if("1".equals(srcPortElementID) || "2".equals(srcPortElementID)) {
    		String videoSource = "PC In";
    		if("2".equals(srcPortElementID))
    			videoSource = "Share";
            ArrayList<String> cmdArray = new ArrayList<String>();
            cmdArray.add("set_networkvideo");      
            cmdArray.add("-video_source");
            cmdArray.add(videoSource);
            System.out.println("ORBApp:setTXConnectionParamaters cmd=" + cmdArray);
            MRLog.getInstance(MRLog.CONSOLE).addInfo("Set Connection Parameters " + cmdArray,MRLog.INFO,MRLog.NOTIFY);
            try {
                MediaRoomServiceUtils.executeCLICommand( cmdArray.toArray(new String[cmdArray.size()]));
            } catch (Exception e) {
                e.printStackTrace();
            }
            ioport = "1";
    	}
    	DeviceAgentUtils.getInstance().setTXConnectionParameters(ioport, streamDoc);
    	return ioport;
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
