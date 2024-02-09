package com.barco.device.base;


import org.dom4j.Document;

import com.barco.device.common.LoggerI;
import com.barco.device.common.Message;
import com.barco.device.common.MessageBusListenerI;
import com.barco.device.common.ServiceException;

public interface DeviceI extends  MessageBusListenerI, LoggerI {
	
	@Override
	public abstract void handleException(ServiceException exception);
	
	@Override
	public abstract void handleMessage(Message message) throws Exception;

	public abstract void setLogLevel(int level);

	public abstract void addLog(String s, int level, int ref);

	public abstract DeviceAdminAgentI getDeviceAdminAgentImpl();

	public abstract LayoutAgentI getLayoutAgentImpl();

	public abstract MediaAgentI getMediaAgentImpl();

	public abstract MediaStoreAgentI getMediaStoreAgentImpl();
	
	public abstract void sendMessage(Message m) throws Exception;

	public abstract void sendRequest(Message m, Document context, ResponseHandlerI responseHandler) throws Exception;

	public abstract void login(String ip, String user, String password, String resource, String managementserver) throws Exception;

	public abstract void logout();
}
