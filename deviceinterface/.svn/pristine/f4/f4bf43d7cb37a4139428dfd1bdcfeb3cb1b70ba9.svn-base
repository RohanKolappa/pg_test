package com.barco.device.base;

import java.io.File;

import org.apache.log4j.Logger;

import com.barco.device.base.LoginInfo.LoginStateEnum;
import com.barco.device.common.MessageBusI;
import com.barco.utils.FileUtils;

/**
 * startLoginDaemon() spawns a thread that performs the MessageBusI.login()
 * repeatedly (interval 5 seconds) until it succeeds without exception,
 * or until stopLoginDaemon() was called.
 * 
 * After login() succeeds, it also calls MessageBusI.sendSCDiscoveryMessage()
 * but ignores exceptions.
 */
public class LoginDaemon implements Runnable {
	private LoginInfo loginInfo;
	Thread deamonthread;
	private MessageBusI messageBus = null;
	Logger logger=null;
	boolean resetThread = false;
	public LoginDaemon(MessageBusI messageBus,Logger logger){
		this.messageBus =messageBus;
		this.logger = logger;
	}

	public void startLoginDaemon(LoginInfo loginInfo){
		this.loginInfo = loginInfo;
		this.loginInfo.setLoginState(LoginStateEnum.Pending.toString());
		logger.debug("LoginDaemon:StartLoginDaemon. UserName:="+loginInfo.getUserName());
		// clean out reset flag
		this.resetThread = false; 
		deamonthread = new Thread(this);
		deamonthread.start();		
	}

	public void stopLoginDaemon(){
		if(this.deamonthread!= null && this.deamonthread.isAlive()) {
			this.deamonthread.interrupt();
			this.resetThread= true; 
		}
		
		try {
			//remove the login status file
			File fileObj = new File(loginInfo.getLoginStatusFile());
			if(fileObj.exists())
				 fileObj.delete();
		} catch (Exception e1) {
			//ignore exception
		}
    	
		String logStatement = "LoginDaemon:StopLoginDaemon. UserName:=";
		if(loginInfo!=null) {
			logStatement +=loginInfo.getUserName();
		}
		logger.debug(logStatement);
	}

	@Override
	public void run() {

		while(!loginInfo.getLoginState().equals(LoginInfo.LoginStateEnum.LoggedIn)) {			
			try {
				logger.debug("LoginDaemon:run login begin ...");
				this.messageBus.login(loginInfo.getUserName(),loginInfo.getPassword(),loginInfo.getResource(),loginInfo.getIpAddress(),loginInfo.getAppserverJID());
				logger.debug("LoginDaemon:run login end");
				loginInfo.setLoginState(LoginInfo.LoginStateEnum.LoggedIn.toString());
				try {
					String loginStatus = "<serverIP>"+loginInfo.getIpAddress()+"</serverIP>";
					logger.debug("LoginDaemon:run login write Status begin ...");
					FileUtils.writeFile(loginInfo.getLoginStatusFile(), loginStatus);
					logger.debug("LoginDaemon:run login write Status end");
				} catch (Exception e) {
					//ignore exception
				}
				
				//send service disco packet
				this.messageBus.sendSCDiscoveryMessage();
				logger.debug("LoginDaemon:Login Success UserName:="+loginInfo.getUserName()+" Wait for SC discovery event");
				break;
			} catch (Exception e) {
				//Login Failed Relogin again
			}
			logger.debug("LoginDaemon:RetryLogin UserName:="+loginInfo.getUserName());
			try{
				//sleep for 5 sec
				Thread.sleep(5000);
			} catch(InterruptedException ie) {
			   resetThread = true;
			}
			
			if(resetThread == true)
				break;
			
		}
		this.resetThread = false;
		logger.debug("LoginDaemon:Exit Thread UserName:="+loginInfo.getUserName());
	}


}