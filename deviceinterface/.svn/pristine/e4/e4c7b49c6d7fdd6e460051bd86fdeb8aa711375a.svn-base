package com.barco.device.base;


import com.barco.device.agent.DeviceAdminAgentI;
import com.barco.device.agent.LayoutAgentI;
import com.barco.device.agent.MediaAgentI;
import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;

public interface DeviceI extends  DeviceFrameworkI {
	
	public abstract DeviceAdminAgentI getDeviceAdminAgentImpl();

	public abstract LayoutAgentI getLayoutAgentImpl();

	public abstract MediaAgentI getMediaAgentImpl();

	public abstract MediaStoreAgentI getMediaStoreAgentImpl();
	
	public abstract void login(String ip, String user, String password, String resource, String managementserver) throws Exception;

	public abstract void logout();
	

	/** see {@link Notifier} */
	public abstract void queueNotification(String key, Message m); 

	/** see {@link Notifier} */
	public abstract void sendNotification(String key, Message m); 

}
