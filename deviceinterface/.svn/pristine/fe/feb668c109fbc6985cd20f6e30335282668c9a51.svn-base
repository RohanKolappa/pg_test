package com.barco.device.base;

import org.apache.log4j.Logger;


public class LoginInfo {

	private String userName;
	private String password;
	private String ipAddress;
	private String resource;
	private String appserverJID;
	private String loginState;
	private String loginStatusFile;
	
	public enum LoginInfoEvents {Shutdown, ConfigChanged, DiscoveredNewSC};
	
	private String lastEvent = "";
	
	private String lastLoggedInServerIP = "";
	
	LoginInfoObserver observer =  null;
	private Logger logger = null;
	
	/**
	 * @param observer
	 *            is notified with a LoginInfoEvents when
	 *            LoginInfo.setLastEvent(LoginInfoEvents e) is called
	 */
	public LoginInfo(LoginInfoObserver observer,Logger logger) {
		this.observer= observer;
		this.logger = logger;
	}
	
	
	public String getLoginStatusFile() {
		return loginStatusFile;
	}
	public void setLoginStatusFile(String loginStatusFile) {
		this.loginStatusFile = loginStatusFile;
	}
	public enum LoginStateEnum {LoggedIn,NotLoggedIn,Pending};

	public String getUserName() {
		return userName;
	}
	public void setUserName(String userName) {
		this.userName = userName;
	}
	public String getPassword() {
		return password;
	}
	public void setPassword(String password) {
		this.password = password;
	}
	public String getIpAddress() {
		return ipAddress;
	}
	public void setIpAddress(String ipAddress) {
		this.ipAddress = ipAddress;
	}
	public String getAppserverJID() {
		return appserverJID;
	}
	public void setAppserverJID(String appserverJID) {
		this.appserverJID = appserverJID;
	}
	
	public String getResource() {
		return resource;
	}
	public void setResource(String resource) {
		this.resource = resource;
	}
	
	public void setLoginInfo(String ip, String user, String password, String resource, String appServerJID) {
		setIpAddress(ip);
		setUserName(user);
		setPassword(password);
		setResource(resource);
		setAppserverJID(appServerJID);
	}
	public String getLoginState() {
		return loginState;
	}
	public void setLoginState(String loginState) {
		//TODO: Update login status file
		this.loginState = loginState;
	}
	
	public synchronized void setLastEvent(LoginInfoEvents loginInfoEvent){		
		this.lastEvent = loginInfoEvent.toString();
		logger.info("LoginInfo: SetLastEvent "+this.lastEvent);
		observer.handleLoginInfoChanged(lastEvent);
	}


	public String getLastLoggedInServerIP() {
		return lastLoggedInServerIP;
	}


	public void setLastLoggedInServerIP(String lastLoggedInServerIP) {
		logger.info("LoginInfo:Set Last Logged in Server IP "+ lastLoggedInServerIP);
		this.lastLoggedInServerIP = lastLoggedInServerIP;
	}

}

interface LoginInfoObserver{
	public void handleLoginInfoChanged(String lastEvent);
}
