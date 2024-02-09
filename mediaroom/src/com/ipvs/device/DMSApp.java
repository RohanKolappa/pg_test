package com.ipvs.device;

import org.dom4j.Document;

import com.ipvs.app.ServiceUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.device.StreamAgents.DeviceConfigStreamAgent;
import com.ipvs.device.StreamAgents.DeviceDestStreamAgent;
import com.ipvs.device.StreamAgents.DeviceSourceStreamAgent;
import com.ipvs.device.http.server.DeviceHttpServerAgent;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.device.mediastore.DeviceMediaStoreAdminServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.utils.FileUtils;

class DMSDeviceAdminServerAgent extends DeviceAdminServerAgent {
	boolean initDone = false;
    DMSDeviceAdminServerAgent(String localConfigFile) {
    	super(localConfigFile);
    }
	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,  String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		if(initDone)
			return;
		initDone = true;
		super.init(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
	}
	
	@Override
	public void reset() throws Exception {
		// Will be called later when userJID changes
	}
	
	public void resetBeforeDelete(MRLog log) {
		try {
			super.reset();
			this.initDone=false;
		}
		catch(Exception e) {
			e.printStackTrace();
        	log.addInfo("DMSDeviceAdminServerAgent::resetBeforeDelete error=" + e.toString());
		}
	}
}

class DMSDeviceConfigStreamAgent extends DeviceConfigStreamAgent {
	boolean initDone = false;
    public DMSDeviceConfigStreamAgent(String resourceID, String localConfigFile,Document agentInitTasksDoc) {
    	super(resourceID, localConfigFile,agentInitTasksDoc);
    }
	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		if(initDone) {
			this.initInternalReliability(log, setServiceDomainResponseDoc);
			return;
		}
		initDone = true;
		super.init(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
	}
	@Override
	public void reset() throws Exception {
		// Will be called later when userJID changes
	}
	
	public void resetBeforeDelete(MRLog log) {
		try {
			super.reset();
		}
		catch(Exception e) {
			e.printStackTrace();
        	log.addInfo("DMSDeviceConfigStreamAgent::resetBeforeDelete error=" + e.toString());
		}
	}
}

class DMSDeviceHttpServerAgent extends DeviceHttpServerAgent{
	boolean initDone = false;
	public DMSDeviceHttpServerAgent(int port, MRLog log) {
    	super(port, log);
    }
	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		if(initDone)
			return;
		initDone = true;
		super.init(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
	}
	@Override
	public void reset() throws Exception {
		// Will be called later when userJID changes
	}
	
	public void resetBeforeDelete(MRLog log) {
		try {
			super.reset();
			this.initDone=false;
		}
		catch(Exception e) {
			e.printStackTrace();
        	log.addInfo("DMSDeviceHttpServerAgent::resetBeforeDelete error=" + e.toString());
		}
	}
	
}


class DMSDeviceSourceStreamAgent extends DeviceSourceStreamAgent {
	boolean initDone = false;

	public DMSDeviceSourceStreamAgent(String localConfigFile,
			DeviceAppI deviceApp) {
		super(localConfigFile, deviceApp);
		// TODO Auto-generated constructor stub
	}
	
	
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		if(initDone)
			return;
		initDone = true;
		super.init(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
	}
	@Override
	public void reset() throws Exception {
		// Will be called later when userJID changes
	}
	
	public void resetBeforeDelete(MRLog log) {
		try {
			super.reset();
		}
		catch(Exception e) {
			e.printStackTrace();
        	log.addInfo("DMSDeviceSourceStreamAgent::resetBeforeDelete error=" + e.toString());
		}
	}
}

class DMSDeviceDestStreamAgent extends DeviceDestStreamAgent {
	boolean initDone = false;
	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		if(initDone)
			return;
		initDone = true;
		super.init(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
	}
	@Override
	public void reset() throws Exception {
		// Will be called later when userJID changes
	}
	
	public void resetBeforeDelete(MRLog log) {
		try {
			super.reset();
		}
		catch(Exception e) {
			e.printStackTrace();
        	log.addInfo("DMSDeviceDestStreamAgent::resetBeforeDelete error=" + e.toString());
		}
	}
}


class DMSDeviceMediaStoreAdminServerAgent extends DeviceMediaStoreAdminServerAgent {
	boolean initDone = false;

	public DMSDeviceMediaStoreAdminServerAgent(String localConfigFile,
			DeviceAppI deviceApp) {
		super(localConfigFile, deviceApp);
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,String userJID,
			Document setServiceDomainResponseDoc) throws Exception {
		if(initDone) {
			this.initInternalReliability();
			return;
		}
		initDone = true;
		super.init(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc);
	}
	@Override
	public void reset() throws Exception {
		// Will be called later when userJID changes
	}
	
	public void resetBeforeDelete(MRLog log) {
		try {
			super.reset();
		}
		catch(Exception e) {
			e.printStackTrace();
        	log.addInfo("DMSDeviceMediaStoreAdminServerAgent::resetBeforeDelete error=" + e.toString());
		}
	}
}


public class DMSApp extends DeviceApp {
    public static final String DMS = "DMS-1000-Device";
    public static final String DMSDEVICEINFO= "/etc/config/hw_profile_dms.txt";
    String prevUserJID = null;

    public static void main(String[] args) {
        DMSApp dms = new DMSApp();        
        dms.processCmdLine(args);
        System.exit(1);
        
    }

    @Override
    public String getDefaultConfigFile() {
    	String deviceInfo = null;
   		try {
			deviceInfo = FileUtils.readFile(DMSDEVICEINFO);
			deviceInfo = deviceInfo.substring(0, deviceInfo.length()-1);
		} catch (Exception e) {
			log.addInfo("Not a valid DMS");
			deviceInfo=DMS;
		}
        return deviceInfo;
    }
    
    protected void resetStreamingServer() {
    	if(srcAgent == null)
    		super.resetStreamingServer();
    }
    

    @Override
    public void addDefaultAgents(String loginResponse) {
    	
    	String currentUserJID = ServiceUtils.getUserJID(loginResponse);
    	
    	// Disable the keep streaming capability till we are able to update the SC with the MediaRoom/Session info
    	// prevUserJID = null;
    	
    	if((lastLoginEvent == XMPP_SHUTDOWN) && (currentUserJID.equals(prevUserJID))) {
        	log.addInfo("DMSApp::addDefaultAgents lastLoginEvent was XMPP_SHUTDOWN reusing Agents"); // Reuse existing Agents
    	}
    	else {
        	log.addInfo("DMSApp::addDefaultAgents lastLoginEvent was NOT XMPP_SHUTDOWN creating new Agents");
        	// reset previous agents if they exist
        	if(cfgAgent != null)
        		((DMSDeviceConfigStreamAgent)cfgAgent).resetBeforeDelete(log);
        	if(srcAgent != null){
        		((DMSDeviceSourceStreamAgent)srcAgent).resetBeforeDelete(log);
        		srcAgent = null;
        	}
        	if(destAgent != null)
        		((DMSDeviceDestStreamAgent)destAgent).resetBeforeDelete(log);
        	//these agent will be initiated irrespective of XMPP login
        	if(httpServerAgent != null)
        		((DMSDeviceHttpServerAgent)httpServerAgent).resetBeforeDelete(log);
        	if(deviceAdminAgent != null)
        		((DMSDeviceAdminServerAgent)deviceAdminAgent).resetBeforeDelete(log);
        	if(mediaStoreAdminServerAgent != null)
        		((DMSDeviceMediaStoreAdminServerAgent)mediaStoreAdminServerAgent).resetBeforeDelete(log);
        	
        	//after reseting the agents reset streaming server.
        	resetStreamingServer();
        	
        	cfgAgent = new DMSDeviceConfigStreamAgent(resourceID, localConfigFile,this.getConfigAgentInitDocument());
        	srcAgent = new DMSDeviceSourceStreamAgent(localConfigFile, this);
        	destAgent = new DMSDeviceDestStreamAgent();
        	mediaStoreAdminServerAgent = new DMSDeviceMediaStoreAdminServerAgent(localConfigFile, this);
    	}
    	
       
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.DEVICEADMINSERVERAGENT, deviceAdminAgent);
        //ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MRServiceConst.XMPPADMINSERVERAGENT, xmppAdminAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT, mediaStoreAdminServerAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.DESTSTREAMAGENT, destAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.SOURCESTREAMAGENT, srcAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.CONFIGSTREAMAGENT, cfgAgent);
        ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), DeviceHttpServerAgent.class.getName(), httpServerAgent);
        
    	if((lastLoginEvent == XMPP_SHUTDOWN) && (currentUserJID.equals(prevUserJID))) {// Mark the agents as ready in the serviceAgentFactory
        	try {
        		ServiceUtils.getServiceClient(this).getMRClientAgent(currentUserJID).getServiceAgentFactory().setAgentStateAsReady(MRServiceConst.DEVICEADMINSERVERAGENT);
        		ServiceUtils.getServiceClient(this).getMRClientAgent(currentUserJID).getServiceAgentFactory().setAgentStateAsReady(MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT);
        		ServiceUtils.getServiceClient(this).getMRClientAgent(currentUserJID).getServiceAgentFactory().setAgentStateAsReady(MediaRoomServiceConst.DESTSTREAMAGENT);
        		ServiceUtils.getServiceClient(this).getMRClientAgent(currentUserJID).getServiceAgentFactory().setAgentStateAsReady(MediaRoomServiceConst.SOURCESTREAMAGENT);
        		ServiceUtils.getServiceClient(this).getMRClientAgent(currentUserJID).getServiceAgentFactory().setAgentStateAsReady(MediaRoomServiceConst.CONFIGSTREAMAGENT);
        		ServiceUtils.getServiceClient(this).getMRClientAgent(currentUserJID).getServiceAgentFactory().setAgentStateAsReady(DeviceHttpServerAgent.class.getName());
        	}
        	catch(Exception e) {
        		e.printStackTrace();
        	}

        }
		prevUserJID = currentUserJID;
    }

    @Override
	public boolean runSSNotificationListener() {
		// TODO Auto-generated method stub
		return true;
	}

	@Override
	public void startHttpServer() {
		httpServerAgent = new DMSDeviceHttpServerAgent(DeviceConst.HTTP_SERVER_PORT, log);
		deviceAdminAgent = new DMSDeviceAdminServerAgent(localConfigFile);
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
