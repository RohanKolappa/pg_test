package com.ipvs.device;

import java.io.File;

import com.ipvs.app.ServiceUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.config.IPVSConst;
import com.ipvs.device.impl.DeviceAppUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.policyadminservice.impl.PolicyAdminAgent;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.FileUtils;

public class AppServerApp implements ServiceMessageHandlerI {
	protected File loginFile;
	protected File domainFile;
	protected String loginStatus = null; 
	protected MRDB db; 
	protected XMPPI xmpp; 
	protected MRResourceFactoryI resourceFactory; 
	protected ServiceAgentFactoryI serviceAgentFactory; 
	protected MRLog log = null; 
	protected String userJID = null; 
	public static boolean testFlag = false;
	protected String localConfigFile = null;
	public AppServerApp(){
        log = MRLog.getInstance(MRLog.DAEMON, true);
        MRClientAgent.setLegacyResponse(false);
	}
    public String getLocalConfigFile() {
        return localConfigFile;
    }
    
	public void setLoginFile(String fname) { 
		loginFile = new File(fname); 
	} 

	public void setServiceDomainFile(String fname) { 
		domainFile = new File(fname); 
	}
	
	public String login(String loginRequest) throws Exception { 
		ServiceClient client = getServiceClient();
		if(IPVSConst.getInstance().isMSComponentLogin()){
		    AppServerAgent.setXmppEnable(true);
		}
		String response = DeviceAppUtils.login(loginRequest,null, client, log);
		userJID = ServiceUtils.getUserJID(response);
		return response;
	} 

	public boolean setServiceDomain() {
		try { 
			String domainRequest = FileUtils.readFile(domainFile); 
			//ServiceUtils.getServiceClient(this).setServiceDomain(userJID, domainRequest);
			ServiceClient client = getServiceClient();
			DeviceAppUtils.setServiceDomain(domainRequest, client,log);
			return true;
				
		} catch (Exception e) { 
			e.printStackTrace(); 
		}
		return false;
	}	
	@Override
	public void handleServiceMessage(String resultMessage, String logMessage) {
		// TODO Auto-generated method stub
		
	}

	public boolean login() { 
	    //moved from DeviceApp.createSession
	    HAUtils.updateHAProperties(log);
		String loginRequest = null; 
		try { 
			loginRequest = FileUtils.readFile(loginFile);
			String loginResponse = ServiceUtils.getServiceClient(this).login(loginRequest);
			//System.out.println(loginResponse);
			if(loginResponse!=null) {
				int state = DeviceAppUtils.getResponseState(loginResponse);
				userJID = ServiceUtils.getUserJID(loginResponse);
				//if(!"".equalsIgnoreCase(userJID)) {
				if(state != 0) {
					return true;
				}
				else {
					return false;
				}
			}	
		} catch (Exception e) { 
			log.addInfo("Caught Exception in processSession" + e.getMessage() ); 
		} 
		return false;

	} 

	public ServiceClient getServiceClient() { 
		return ServiceUtils.getServiceClient(this); 
	} 

	public static void main(String[] args) { 
		AppServerApp app = new AppServerApp(); 
		app.processCmdLine(args); 

	} 
	
	public void processCmdLine(String[] args) { 
		boolean bStartAgent = false; 
		boolean bUsage = false; 
		String loginFile = null; 
		String servicedomainFile = null; 
		for (int i = 0 ; i < args.length; i++) { 
			if (args[i].equals("login")) { 
				bStartAgent = true; 

				if (args.length < (i + 1)) { 
					bUsage = true; 

					break; 
				} else { 
					i++; 
					loginFile = args[i]; 
				} 
			} else if (args[i].equals("servicedomain")) { 
				bStartAgent = true; 

				if (args.length < (i + 1)) { 
					bUsage = true; 

					break; 
				} else { 
					i++; 
					servicedomainFile = args[i]; 
				} 
            } else if (args[i].equals("config")) {
                bStartAgent = true; 

                if (args.length < (i + 1)) { 
                    bUsage = true; 

                    break; 
                } else { 
                    i++; 
                    localConfigFile = args[i]; 
                } 
			}  
		} 

		
		
		if (bUsage) { 
			System.out.println("Usage"); 
			System.out.println("login <loginfilename> " + "servicedomain <servicedomainfilename> "); 
			return; 
		} 
		
		File file = new File(loginFile);
		if(!file.exists()) {
			System.out.println("Unable to locate file " + loginFile);
			bStartAgent = false;
		}
		file = new File(servicedomainFile);
		if(!file.exists()) {
			System.out.println("Unable to locate file " + servicedomainFile);
			bStartAgent = false;
		}
		file = null;
		if (bStartAgent) { 
			try { 
				setLoginFile(loginFile); 
			} catch (Exception e) { 
				e.printStackTrace(); 
			} 
			try { 
				setServiceDomainFile(servicedomainFile); 
			} catch (Exception e) { 
				e.printStackTrace(); 
			}
			login();
			addDefaultAgents();
			setServiceDomain();
		} 
	} 
	
	public void addDefaultAgents() { 
		ConfigStreamAgent  configStreamAgent = new ConfigStreamAgent();
		AppServerAgent appserverAgent = new AppServerAgent();
		PolicyAdminAgent policyAgent = PolicyAdminAgent.getPolicyAdminAgent();
		ServiceUtils.getServiceClient(this).addServiceAgent(userJID, ConfigStreamAgent.class.getName(), configStreamAgent);
		ServiceUtils.getServiceClient(this).addServiceAgent(userJID, PolicyAdminAgent.class.getName(), policyAgent);
		ServiceUtils.getServiceClient(this).addServiceAgent(userJID, AppServerAgent.class.getName(), appserverAgent);
		MediaStoreServiceConst.init();
		AppServerAgent.setXmppEnable(true);
		AppServerAgent.setLocalConfigFile(localConfigFile);
	} 

}
