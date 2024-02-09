package com.ipvs.device;

import java.io.File;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Node;

import com.ipvs.app.ServiceUtils;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.device.handler.SetServiceClientConfigRequestHandler;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.AgentInitUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.device.StreamAgents.DeviceDestStreamAgent;
import com.ipvs.device.StreamAgents.DeviceSourceStreamAgent;
import com.ipvs.device.http.server.DeviceHttpServerAgent;
import com.ipvs.device.impl.ConfigListener;
import com.ipvs.device.impl.DeviceAgentUtils;
import com.ipvs.device.impl.DeviceAppUtils;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.device.impl.DeviceStreamingServerImpl;
import com.ipvs.device.impl.SSNotificationListener;
import com.ipvs.device.mediastore.DeviceMediaStoreAdminServerAgent;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.ConfigDeviceUtils;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public abstract class DeviceApp implements ServiceMessageHandlerI, DeviceAppI {
	
	public static final int XMPP_SHUTDOWN = 1;
	public static final int LOGIN_UPDATE = 2;
	public static final int VERSION_MISMATCH = 3;
	public static final int CONFIG_INIT_FAILED = 4;
	
	
	protected int lastLoginEvent = 0;
	
    protected String deviceType = null;
    protected MRLog log = null;
    protected String userJID = null;

    //Config declaration
    private File loginFile;
    private File domainFile;
    protected String localConfigFile = null;

    //agent declaration
    protected ConfigStreamAgent cfgAgent;
    protected DeviceSourceStreamAgent srcAgent;
    protected DeviceDestStreamAgent destAgent;
    protected DeviceAdminServerAgent deviceAdminAgent;
    protected DeviceMediaStoreAdminServerAgent mediaStoreAdminServerAgent;
    protected DeviceHttpServerAgent httpServerAgent;

    //Thread Declaration   
    private SSNotificationListener ssNotificationListener = null;
    private ConfigListener configListener = null;
    private Thread configListenerThread = null;
    private Thread ssNotificationListenerThread = null;
    
    //waiting for appserver jid
    private boolean appserverAvailable = true;

	//session variables
    private long modificationTime;
    private boolean resetSession;
    private String statusFile = null;
    protected String resourceID;
  	//by default enableXMPPArgument is false... that means ms is running as component.
    private boolean disableAppServerAgentArgument = false;
    private static final String STATUS_INITIALIZING_FILE ="/var/status/DeviceApp.java.initializing";

    public DeviceApp() {
        localConfigFile = "/etc/config/Device.xml";
        log = MRLog.getInstance(MRLog.DAEMON, true);
        log.setLogLevel(MRLog.INFO);
        statusFile = getStatusFile();
        MRClientAgent.setLegacyResponse(false);
        configListener = new ConfigListener(log, this);
        log.addInfo("DeviceApp Constructor", MRLog.INFO, MRLog.NOTIFY);
        if (runSSNotificationListener()) {
            log.addInfo("DeviceApp Starting Notification Listener", MRLog.INFO, MRLog.NOTIFY);
            ssNotificationListener = new SSNotificationListener(log, this);
        }
    }
    
    public String getUserJID() {
        return userJID;
    }

    public void setUserJID(String userJID) {
        this.userJID = userJID;
    }

    public String getLocalConfigFile() {
        return localConfigFile;
    }

    public void setLocalConfigFile(String localConfigFile) {
        this.localConfigFile = localConfigFile;
    }

    // virtual functions
    abstract public String getDefaultConfigFile();
    abstract public boolean runSSNotificationListener();

    abstract public void addDefaultAgents(String loginResponse);

    public void listattr(int ts, Element e) {
        for (int i = 0; i < ts; i++) {
            System.out.print("\t");
        }

        System.out.println("<" + e.getName() + ">");

        Iterator<?> iter = e.elements().iterator();

        if (!iter.hasNext()) {
            System.out.println(e.getStringValue());
        }

        ts++;

        while (iter.hasNext()) {
            listattr(ts, (Element) iter.next());
        }

        ts--;

        for (int i = 0; i < ts; i++) {
            System.out.print("\t");
        }

        System.out.println("</" + e.getName() + ">");
    }

    public String getUser(Document doc) {
        String val = null;
        Element el = null;

        try {
            el = MRXMLUtils.getElementXpath(doc, DeviceConst.USERPATH);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (el != null) {
            val = el.getStringValue();
        }

        return val;
    }

    public String getServer(Document doc) {
        String val = null;
        Element el = null;

        try {
            el = MRXMLUtils.getElementXpath(doc, DeviceConst.SERVERPATH);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (el != null) {
            val = el.getStringValue();
        }

        return val;
    }

    public String getResource(Document doc) {
        String val = null;
        Element el = null;

        try {
            el = MRXMLUtils.getElementXpath(doc, DeviceConst.RESOURCEPATH);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (el != null) {
            val = el.getStringValue();
        }

        return val;
    }

    public String getDomain(Document doc) {
        String val = null;
        Element el = null;

        try {
            el = MRXMLUtils.getElementXpath(doc, DeviceConst.DOMAINPATH);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (el != null) {
            val = el.getStringValue();
        }

        return val;
    }

    public String getPassword(Document doc) {
        String val = null;
        Element el = null;

        try {
            el = MRXMLUtils.getElementXpath(doc, DeviceConst.PASSWORDPATH);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (el != null) {
            val = el.getStringValue();
        }

        return val;
    }

    public String getPort(Document doc) {
        String val = null;
        Element el = null;

        try {
            el = MRXMLUtils.getElementXpath(doc, DeviceConst.PORTPATH);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (el != null) {
            val = el.getStringValue();
        }

        return val;
    }

    public void parseLogin() {
        Document doc = null;

        try {
            String xml = FileUtils.readFile(loginFile);
            doc = MRXMLUtils.getDocument(xml);
        } catch (Exception e) {
            StackTraceElement[] st = e.getStackTrace();

            for (int i = 0; i < st.length; i++) {
                System.out.println(st[i].toString());

                if (i > 5) {
                    break;
                }
            }
        }

        resourceID = getResource(doc);
    }

    public void parseDomain() {
        //Document doc = null;
        try {
            //String xml = TestUtils.readFile(domainFile);
            //doc = MRXMLUtils.getDocument(xml);
        } catch (Exception e) {
            StackTraceElement[] st = e.getStackTrace();

            for (int i = 0; i < st.length; i++) {
                System.out.println(st[i].toString());

                if (i > 5) {
                    break;
                }
            }
        }

        try {
            //servicedomain = MRXMLUtils.getValueXpath(doc, "//domain", "");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void upgradeConfig(String configFile, String handlerFile, Document defaultDocument, String newConfigFile, boolean bClearLists) {
        Document doc = null;
        Document dochandler = null;
        Document docbasecfg = null;
        Document docrefcfg = null;

        try {
            docbasecfg = MRXMLUtils.elementToDocument(defaultDocument.getRootElement());
            docrefcfg = MRXMLUtils.elementToDocument(defaultDocument.getRootElement());

            clearlists(docbasecfg, "//RouteEntry");
            clearlists(docbasecfg, "//EthernetPortConfig");
            clearlists(docbasecfg, "//NASEntry");
            clearlists(docbasecfg, "//HostEntry");
            clearlists(docbasecfg, "//NATZoneEntry");

            if (bClearLists) {
                // TBR once we have CLI interface to delete lists
                clearlists(docbasecfg, "//V2DCodecVideoLookupEntryConfig");
                clearlists(docbasecfg, "//V2DDecoderVideoLookupEntryConfig");
            }
        } catch (Exception e1) {
            e1.printStackTrace();
        }

        String xml = null;
        log.addInfo("UpgradeConfig :" + configFile + " ->" + handlerFile);

        try {
            dochandler = MRXMLUtils.loadXMLResource(handlerFile, log);
        } catch (Exception e1) {
            e1.printStackTrace();
        }

        try {
            xml = FileUtils.readFile(new File(configFile));
            doc = MRXMLUtils.getDocument(xml);
        } catch (Exception e) {
            System.out.println("upgradeConfig :" + e.toString());
        }

        if (doc != null) {
            Element el = null;
            Iterator<?> iter = doc.getRootElement().elements().iterator();

            while (iter.hasNext()) {
                el = (Element) iter.next();
                configListener.updateConfig(el, dochandler, docbasecfg, docrefcfg);
            }
        }

        try {
            FileUtils.writeFile(newConfigFile, MRXMLUtils.documentToString(docbasecfg));
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void setLoginFile(String fname) {
        loginFile = new File(fname);
        modificationTime = 0;

        if (loginFile.exists()) {
            modificationTime = loginFile.lastModified();
        }
    }

    public void setServiceDomainFile(String fname) {
        domainFile = new File(fname);
    }

    public ServiceClient getServiceClient() {
        return ServiceUtils.getServiceClient(this);
    }
    
    //reset the streaming server
    protected void resetStreamingServer() {
    	log.addInfo("Reset StreamingServer",MRLog.INFO,MRLog.NOTIFY);
    	StringBuffer resetCmd = new StringBuffer();
    	resetCmd.append("action=reset");
        try {
            String commandResult = DeviceStreamingServerImpl.executeCommand(resetCmd.toString());
            if ((commandResult == null) || commandResult.startsWith("201 OK")) {
                log.addInfo("StreamingServer error cmd=" + resetCmd);
                throw new MRException(MediaRoomServiceExceptionCodes.EXECUTESTREAMINGSERVERCMDFAILED, "EXECUTE STREAMING SERVER COMMAND FAILED: RESULT = " + commandResult + " cmd=" + resetCmd);
            }
        } catch (MRException exp) {
            exp.printStackTrace();
        } catch (Exception exp) {
            exp.printStackTrace();
        }
    	
    }

    public String login(String loginRequest,String linkLocalLoginRequest) throws Exception {
    	
    	//loginRequest = HAUtils.updateLoginRequest(loginRequest);
    	this.resetStreamingServer();          
        ServiceClient client = getServiceClient();
        log.addInfo("Login started :" + this.getClass().getName() +" run appserveragent as component " + (!disableAppServerAgentArgument));
        if (disableAppServerAgentArgument) {
            log.addInfo("Force XMPPServerEnable=false since disableAppServerAgentArgument=" + disableAppServerAgentArgument);
        } else {
            Document configDoc = MRXMLUtils.loadXMLFile(localConfigFile, log);
            String xmppEnable = MRXMLUtils.getValueXpath(configDoc, "//XMPPServerEnable", "false");
            
            boolean xmppServerEnable = Boolean.parseBoolean(xmppEnable);
            //update xmpp server enabled flag based on ha running or not
            xmppServerEnable = HAUtils.enableAppserverAgentOrNot(xmppServerEnable);
            
            AppServerAgent.setXmppEnable(xmppServerEnable);
            AppServerAgent.setLocalConfigFile(localConfigFile);   
        }
        String response = DeviceAppUtils.login(loginRequest,linkLocalLoginRequest, client, log);
        String userJID = ServiceUtils.getUserJID(response);
        
        //special case to handle xmpp login failed
        if(MRClientUtils.isLinkLocalJID(userJID)){
        	addDefaultAgents(response);
        	throw new MRException(MRExceptionCodes.XMPPLOGINFAILED, "XMPP Login Failed");
        }
        
        setUserJID(userJID);
        addDefaultAgents(response);
        
        return response;
    }

    public void setServiceDomain(String domainRequest) throws Exception {
        log.addInfo("setdomain started");
        try {
        	ServiceClient client = getServiceClient();        
            DeviceAppUtils.setServiceDomain(domainRequest, client,log);        
        } catch(MRException exp) {
        	if(exp.getCode() == MediaRoomServiceExceptionCodes.APPSERVER_JID_NOT_FOUND) {
        		handleAppServerUnavailable();        		           		
        	} else {
        	 	exp.printStackTrace();                     
        		setResetSession(true);
        		throw exp;
        	}
        }catch (Exception e) {
        	//e.printStackTrace();                
        	setResetSession(true);
        	throw e;
        }
        log.addInfo("setdomain complete");
        
    }

    private String getStatusFile() {
        try {
            if (IPVSConst.getInstance().getOS().equals("windows")) {
                return DeviceConst.STATUSFILE_WINDOWS;
            }
        } catch (Exception ex) {
        }

        return DeviceConst.STATUSFILE_UNIX;
    }
    
    public Boolean initializeMRClientAgent(String userJID) {
    	try {
    		if (!this.getServiceClient().getMRClientAgent(this.userJID).getInitialized()) {
        		log.addInfo("HandleAgentConfigUpdates MRClientAgent not initialized ");
				return false;
			}
		} catch (MRException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
    		log.addInfo("HandleAgentConfigUpdates MRClientAgent not initialized ");
			return false;
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
    		log.addInfo("HandleAgentConfigUpdates MRClientAgent not initialized ");
			return false;
		}
		return true;
    }
    public void handleAgentConfigUpdate(String updateParentXPath,  String  updateXPath) {
    	
    	if(!initializeMRClientAgent(userJID))
    		return;
    	
    	try {
    		if (cfgAgent != null) {
    			cfgAgent.handleConfigUpdates(localConfigFile,updateParentXPath, updateXPath,  userJID, this);
    		}

    		if (srcAgent != null) {
    			srcAgent.handleConfigUpdates(localConfigFile, updateParentXPath,updateXPath, userJID, this);
    		}
    	} catch (MRException exp) {
    		exp.printStackTrace();
    		log.addInfo("HandleAgentConfigUpdates Failed ");
    		log.addInfo("ERROR : " + exp.getMessage());
    	} catch (Exception exp) {
    		exp.printStackTrace();
    		log.addInfo("HandleAgentConfigUpdates Failed ");
    		log.addInfo("ERROR : " + exp.getMessage());
    	}
    }
        
    public void handleAgentStatusUpdate(String updateXPath) {
    	
    	if(!initializeMRClientAgent(userJID))
    		return;
    	
    	try {
    		if (cfgAgent != null) {
    			cfgAgent.handleStatusUpdates(updateXPath, userJID, this);
    		}

    	} catch (MRException exp) {
    		exp.printStackTrace();
    		log.addInfo("HandleAgentStatusUpdates Failed ");
    		log.addInfo("ERROR : " + exp.getMessage());
    	} catch (Exception exp) {
    		exp.printStackTrace();
    		log.addInfo("HandleAgentStatusUpdates Failed ");
    		log.addInfo("ERROR : " + exp.getMessage());
    	}
    }
    
    public void handleAgentErrorStatusUpdate(Element element, String state) {
    	
    	if(!initializeMRClientAgent(userJID))
    		return;
    	
    	try {
    		if (cfgAgent != null) {
    			cfgAgent.handleErrorStatusUpdates(element, state, userJID, this);
    		}

    	} catch (MRException exp) {
    		exp.printStackTrace();
    		log.addInfo("handleAgentErrorStatusUpdate Failed ");
    		log.addInfo("ERROR : " + exp.getMessage());
    	} catch (Exception exp) {
    		exp.printStackTrace();
    		log.addInfo("handleAgentErrorStatusUpdate Failed ");
    		log.addInfo("ERROR : " + exp.getMessage());
    	}
    }
    
    public void handleServiceMessage(String userJID, String message) {
    
        try {        	
            if (ServiceAgentUtils.isShutdownEvent(message)) {
            	lastLoginEvent = XMPP_SHUTDOWN;
                setResetSession(true);
                return;
            }
            
            //if set service domain request failed and if the failure is not service version mismatch then reset the session
            int setServiceDomainState = ServiceAgentUtils.getSetServiceDomainState(message, log);
            if(setServiceDomainState != MRRequest.STATE_DONE && setServiceDomainState != MRExceptionCodes.SERVICE_VERSION_MISMATCH) {
            	setResetSession(true);
            	lastLoginEvent = VERSION_MISMATCH;
            	return;
            }
            
            
            //if config stream agent init failed reset the session
            if(cfgAgent != null && cfgAgent.isAgentInitFailed()) {
            	setResetSession(true);
            	lastLoginEvent = CONFIG_INIT_FAILED;
            	return;
            }
            
        	String agentMessage = MRXMLUtils.getValueXpath(MRXMLUtils.stringToDocument(message), "//notificationtype", "");
         	if(agentMessage.equals(ServiceClient.XMPPEVENT)){         		
        		if(AppServerAgentUtils.isServiceResourceUpdateEvent(message,log,ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getDb(),this.userJID,
        				this.getServiceClient().getMRClientAgent(this.userJID).getServiceAgentFactory())) {        		
        			handleAppServerAvailable();
                    return;
        		}
        	}

        } catch (Throwable throwableObj) {
            throwableObj.printStackTrace();
        }
    }

 
    /**
     * If AppServer is not ready then wait for service resource update and resent the set service domain
	 * If AppServer already logged into the system but some reason AppServer got reset 
     * then logout the users and re login into the system
     * @throws Exception
     */
    private void handleAppServerAvailable() throws Exception {
       if(!isAppServerAvailable()){
			System.out.println("****** Appserver ready. Send Set service Domain request again ******");
			log.addInfo("Appserver ready. Send Set service Domain request again", MRLog.INFO, MRLog.NOTIFY);
			appServerAvailable(true);			
			this.setServiceDomain(getDomainRequest());
		} else {
			System.out.println("***** Appserver resets the connection. Logout from the system and login again ******");
			log.addInfo("Appserver resets the connection. Logout from the system and login again", MRLog.INFO, MRLog.NOTIFY);            			
			//setResetSession(true);
		}  
	}
    
    /**
     *
     */
    private void handleAppServerUnavailable() {		
    	System.out.println("******** Waiting For Appserver To Be Ready **************");
		log.addInfo("Waiting For App Server To  Be Ready", MRLog.INFO, MRLog.NOTIFY);
		this.appServerAvailable(false); 
	}

	public boolean clearlists(Document doc, String trigger) {
        Element el = null;
        List<?> l = null;

        try {
            l = MRXMLUtils.getListXpath(doc, trigger);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if ((l != null) && !l.isEmpty()) {
            for (int i = 0; i < l.size(); i++) {
                el = (Element) l.get(i);
                el.detach();
            }
        }

        return true;
    }
	
	public boolean clearEmptyNodes(Document doc, String elementName) {
		Element el = null;
		List<?> elementNameList = null;
		boolean isEmptyElement= true;

		try {
			elementNameList = MRXMLUtils.getListXpath(doc, elementName);
		} catch (Exception e) {
			e.printStackTrace();
		}

		if ((elementNameList != null) && !elementNameList.isEmpty()) {
			for (int i = 0; i < elementNameList.size(); i++) {
				el = (Element) elementNameList.get(i);
				isEmptyElement=true;
				List<?> list = MRXMLUtils.getChildren(el);
				for (Object object : list) {
					Element el1 = (Element) object;
					if(!((el1.getNodeType() == Node.ELEMENT_NODE) && el1.getTextTrim().isEmpty())){
						isEmptyElement = false;
						break;
					}
				}
				if(isEmptyElement)
					el.detach();
									
			}
			
		}

		return true;
	}

    public void handleConfig(Document docconfig, String handlerFile) {
        ConfigDeviceUtils.initDirtyChildrenFlag(docconfig, log);
        ConfigDeviceUtils.handleConfig(docconfig, handlerFile, log);
    }

    public static void main(String[] args) {
        //System.out.println("In Device App");
    }

    public void processCmdLine(String[] args) {
        boolean bStartAgent = false;
        boolean bUsage = false;

        String loginFile = null;
        String servicedomainFile = null;
        String configFile = null;

        if (args.length < 1) {
            bUsage = true;
        }

        for (int index = 0; index < args.length; index++) {
            String command = args[index];

            if (DeviceConst.UPGRADE_CONFIG.equals(command) || DeviceConst.FACTORY_CONFIG.equals(command)) {
                if (args.length < (index + 2)) {
                    bUsage = true;

                    break;
                }

                handleConfigCommands(args, index, command);

                return;
            } else if (DeviceConst.APPLY_CONFIG.equals(command)) {
                if (args.length < (index + 1)) {
                    bUsage = true;

                    break;
                }

                handleApplyConfig(args, index);

                //TBD: Confirm with Shanker why he is doing join after apply config
                // join();
                return;
            } else if (command.equals("login")) {
                bStartAgent = true;

                if (args.length < (index + 1)) {
                    bUsage = true;

                    break;
                } else {
                    index++;
                    loginFile = args[index];
                }
            } else if (command.equals("servicedomain")) {
                bStartAgent = true;

                if (args.length < (index + 1)) {
                    bUsage = true;

                    break;
                } else {
                    index++;
                    servicedomainFile = args[index];
                }
            } else if (command.equals("config")) {
                if (args.length < (index + 1)) {
                    bUsage = true;

                    break;
                } else {
                    index++;
                    configFile = args[index];
                    localConfigFile = configFile;
                }
            } else if (command.equals("disableAppServerAgent")) {
                if (args.length < (index + 1)) {
                    bUsage = true;
                    break;
                } else {
                    index++;
                    //by default disableAppServerAgent is true... that means ms is running as component.
                    disableAppServerAgentArgument = Utils.booleanValue(args[index], true);
                }                
            } else {
                bUsage = true;
            }
        }

        if (bUsage) {
            System.out.println("Options");
            System.out.println("UpgradeConfig <oldconfig> <newconfig>");
            System.out.println("FactoryConfig <oldconfig> <newconfig>");
            System.out.println("disableAppServerAgent=true/false  to run appserveragent as component in separate JVM");
            System.out.println("login <loginfilename> " + "servicedomain <servicedomainfilename> " + "config <configfilename> [disableAppServerAgent true]");

            return;
        }
        
        try {
        	FileUtils.removeFileOrDir(STATUS_INITIALIZING_FILE);
		} catch (Exception e1) {
			e1.printStackTrace();
		}

        if (bStartAgent) {
            try {
                setLoginFile(loginFile);
            } catch (Exception e) {
                e.printStackTrace();
            }

            try {
                Document deviceDoc = MRXMLUtils.getDocument(new File(localConfigFile));
                deviceType = deviceDoc.getRootElement().getName();
            } catch (Exception e) {
                e.printStackTrace();
            }

            try {
                setServiceDomainFile(servicedomainFile);
            } catch (Exception e) {
                e.printStackTrace();
            }

            handleSession();
        }
    }

    private String getLoginRequest() {
        try {
        	modificationTime = loginFile.lastModified();
            return FileUtils.readFile(loginFile);
        } catch (Exception e1) {
            log.addInfo("Caught Exception in getLoginRequest" + e1.getMessage());

            return null;
        }
    }

    private String getDomainRequest() {
        try {
            return FileUtils.readFile(domainFile);
        } catch (Exception e1) {
//            e1.printStackTrace();

            return null;
        }
    }
    
    
    private boolean isLinkLocalEnabled() {
    	  try {
              Document deviceDoc = MRXMLUtils.getDocument(new File(localConfigFile));
              deviceType = deviceDoc.getRootElement().getName();
              System.out.println("Link Local Service Enabled :="+MRXMLUtils.getValueXpath(deviceDoc, "//LinkLocalXMPPAgentServiceConfig/Enable", "false"));
              return Utils.getBoolean(MRXMLUtils.getValueXpath(deviceDoc, "//LinkLocalXMPPAgentServiceConfig/Enable", "false"),false);
          } catch (Exception e) {
        	  return false;
          }   	
    }
    
    private String getLinkLocalLoginRequest() throws Exception{
    	Document deviceDoc = null;
	
		deviceDoc = MRXMLUtils.getDocument(new File(localConfigFile));
		deviceType = deviceDoc.getRootElement().getName();    
	    String macAddress = DeviceAppUtils.getMacAddress(log);
	    String deviceName = MRXMLUtils.getValueXpath(deviceDoc, "//XMPPAgentServiceConfig/DeviceName", "");     	    
	    return MRClientUtils.getLinkLocalRequest(macAddress, macAddress, deviceType,deviceName,false);    	
    }

    /**
     * Login to the XMPP
     * @param loginRequest
     * @param domainRequest
     */
    private void createSession() {
        String loginStatus = null;
        boolean isLogin = true;
        
      
        String loginRequest = null;
        String linkLocalLoginRequest = null;      
        
        try {
			loginRequest = getLoginRequest();
		} catch (Exception e2) {		
			log.addInfo("Login Request Not Found");
		}
		
		
		if(isLinkLocalEnabled()) {			
			try {
				linkLocalLoginRequest = getLinkLocalLoginRequest();
			} catch (Exception e) {
				log.addInfo("Link Local Login Request Not Found");
				//e.printStackTrace();
				setResetSession(true);
				return;
			}
		}
        
        if (loginRequest != null || linkLocalLoginRequest !=null) {
           if(loginRequest!=null)
        	    parseLogin();

            try {
                loginStatus = login(loginRequest,linkLocalLoginRequest);
            } catch (Exception e1) {
                e1.printStackTrace();
                setResetSession(true);
                isLogin = false;
            }

            if (loginStatus != null) {
                System.out.println(loginStatus);
                try {
                	//writing server IP to status file
                	 Document loginRequestDoc = MRXMLUtils.stringToDocument(loginRequest);
         	         Element loginReqiestDataEl = MRXMLUtils.getElementXpath(loginRequestDoc, "//" + MRConst.LOGIN_REQUEST_DATA);
         	         String serverIP = MRXMLUtils.getValue(loginReqiestDataEl, "server", "");
         	         loginStatus = StringUtils.replace(loginStatus, "</version>", "</version><serverIP>"+serverIP+"</serverIP>");
                     FileUtils.writeFile(statusFile, loginStatus);
                } catch (Exception e) {
                   // e.printStackTrace();
                }
            } else {
                // Remove the status file that reflects the state
            }
        } else {
        	System.out.println("Login File Not Exists");
        	setResetSession(true);  
        	return;
        }

        String domainRequest = getDomainRequest();        
        
        if ((domainRequest != null) && (loginStatus != null)) {
            parseDomain();

            try {
            	setServiceDomain(domainRequest);
            }catch (Exception e) {
            	e.printStackTrace();
            	isLogin = false;
            }
        }
       
        if (isLogin) {
        	setResetSession(false);
        }
    }

    /**
     * Logout from xmpp
     */
    private void deleteSession() {
        try {
            FileUtils.deleteFile(this.statusFile);
        } catch (Exception e) {
            // Ignore Exception Shanker :10/10
        }
        
        //delete API Key
        
        try {
            FileUtils.deleteFile(DeviceConst.API_KEY_TABLE_FILE_PATH);
        } catch (Exception e) {
            // Ignore Exception Shanker :10/10
        }

        this.resetStreamingServer();
        ServiceClient client = getServiceClient();

        if (client != null) {
            String logoutResponse = client.logout(userJID);

            if (logoutResponse != null) {
                System.out.println(logoutResponse);
            }
        }
    }

    /**
     * Handles the login session
     * Maintains the login session and checks the login file update on device
     * Starts the CLI and Streaming Server Notification threads
     * If some reason logout happened then stops the CLI, Streaming Server threads and closes the session.
     */
    private void handleSession() {
    	this.startHttpServer();
        while (true) {
            try {            
                
                HAUtils.updateHAProperties(log);

                createSession();
                startCLIOutputThread();
                
                if (runSSNotificationListener()) {
                    startSSNotificationThread();
                }
                
                watchTheSession();

                if (runSSNotificationListener()) {
                    stopSSNotificationThread();
                }
                stopCLIThread();
                deleteSession();
                
            } catch (Exception exp) {
               // exp.printStackTrace();
            }
        }
    }

    /**
     * Watch the login session.
     * Checks the login file updates and server shutdown events.
     */
    private void watchTheSession() {
        while (modificationTime <= loginFile.lastModified()) {
            
            try {
                Thread.sleep(5000);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            if (isResetSession()) {            	
                break;
            }

            if (loginFile.exists()) {
                if (modificationTime < loginFile.lastModified()) {
                	log.addInfo("Updated login File. ModificationTime:="+modificationTime +" loginFileModification:="+loginFile.lastModified());
                    modificationTime = loginFile.lastModified();
                    lastLoginEvent = LOGIN_UPDATE;
                    break;
                }
            } else {
            	log.addInfo("login File not exist");
                break;
            }
        }
    }

    public boolean isResetSession() {
        return resetSession;
    }

    public synchronized void setResetSession(boolean resetSession) {
        this.resetSession = resetSession;
        log.addInfo("Reset the Session. ResetSession:="+this.resetSession);
    }

    /**
    * Handle Apply Config
    * @param args
    * @param index
    */
    private void handleApplyConfig(String[] args, int index) {
        index++;

        String configFile = args[index];
        //System.out.println(args[index]);

        Document doc = null;

        File f = new File(configFile);

        try {
            doc = MRXMLUtils.getDocument(f);
        } catch (Exception e) {
            e.printStackTrace();
        }

        if (doc != null) {
            try {
            	
            	// check if the node or entry is empty than delete that entry and re-write it on the disk
            	clearEmptyNodes(doc, "//NASEntry");
                clearEmptyNodes(doc, "//HostEntry");
                clearEmptyNodes(doc, "//NATZoneEntry");
                try {
                    FileUtils.writeFile(configFile, MRXMLUtils.documentToString(doc));
                } catch (Exception e) {
                    e.printStackTrace();
                }
                handleConfig(doc, DeviceConst.CFGHANDLER);
            } catch (Exception e) {
                //e.printStackTrace();
                System.out.println(e.toString());
            }
        }
    }

    /**
     * Handle Upgrade Config and factory Config (Revert To Default)
     * @param args
     * @param index
     * @param command
     */
    private void handleConfigCommands(String[] args, int index, String command) {
        String legacyConfigFile = null;
        String newConfigFile = null;
        String defaultConfigFile = null;
        Document defaultDocument = null;
        String defaultConfig = null;

        try {
            index++;
            legacyConfigFile = args[index];
            System.out.println(args[index]);

            index++;
            newConfigFile = args[index];
            System.out.println(args[index]);

            if (args.length == (index + 2)) {
                // default config file provided on command line
                index++;
                defaultConfigFile = args[index];
                System.out.println(args[index]);

                defaultDocument = MRXMLUtils.loadXMLFile(defaultConfigFile, log);
            } else {
                // default config file picked up from objectXml
                defaultConfig = this.getDefaultConfigFile();
                defaultConfigFile = DeviceConst.BASECFGPREFIX + defaultConfig + DeviceConst.BASECFGSUFFIX;

                defaultDocument = MRXMLUtils.loadXMLResource(defaultConfigFile, log);
            }

            if (command.equals(DeviceConst.UPGRADE_CONFIG)) {
                System.out.println("Translating " + legacyConfigFile + "->" + newConfigFile);
                upgradeConfig(legacyConfigFile, DeviceConst.CFGHANDLER, defaultDocument, newConfigFile, true);
            } else if (command.equals(DeviceConst.FACTORY_CONFIG)) {
                System.out.println("Translating " + legacyConfigFile + "->" + newConfigFile);
                upgradeConfig(legacyConfigFile, DeviceConst.CFGHANDLER, defaultDocument, newConfigFile, false);
            }
        } catch (Exception e1) {
            e1.printStackTrace();
        }
    }

    /**
     * Stop CLI Output Thread
     */
    private void stopCLIThread() {
        ConfigListener.setResetCLI(true);

        try {
            configListenerThread.join();
        } catch (Exception exp) {
            //ignore exception 
        }

        log.addInfo("Stoped CLI Thread", MRLog.INFO, MRLog.NOTIFY);
    }

    /**
     * Start CLI Output Thread
     */
    private void startCLIOutputThread() {
        ConfigListener.setResetCLI(false);

        if (configListener == null) {
            configListener = new ConfigListener(log, this);
        }

        configListenerThread = new Thread(configListener, "CLIThread");
        configListenerThread.setName("CLIThread");
        configListenerThread.start();
        log.addInfo("Started CLI Thread", MRLog.INFO, MRLog.NOTIFY);
    }

    /**
     * Start the Streaming Server Notification Thread
     */
    private void startSSNotificationThread() {
        SSNotificationListener.setResetNotification(false);

        if (ssNotificationListener == null) {
            ssNotificationListener = new SSNotificationListener(log, this);
        }

        ssNotificationListenerThread = new Thread(ssNotificationListener, "SSNotificationThread");
        ssNotificationListenerThread.start();
        log.addInfo("Started SSNotificationThread", MRLog.INFO, MRLog.NOTIFY);
    }

    /**
     * Stop SSNotificationThread
     */
    private void stopSSNotificationThread() {
        SSNotificationListener.setResetNotification(true);

        try {
            ssNotificationListenerThread.join();
        } catch (Exception exp) {
            //Ignore Exception
        }

        log.addInfo("Stoped SSNotificationThread", MRLog.INFO, MRLog.NOTIFY);
    }
    
    
    public boolean isAppServerAvailable() {
		return appserverAvailable;
	}

	public synchronized void appServerAvailable(boolean isWaitingForAppserver) {
		this.appserverAvailable = isWaitingForAppserver;
	}
	
    public String setTXConnectionParameters(String srcPortElementID, Document streamDoc) {
    	DeviceAgentUtils.getInstance().setTXConnectionParameters(srcPortElementID, streamDoc);
    	return srcPortElementID;
    }
    
    //returns the agent init task to be done
    protected Document getConfigAgentInitDocument() {
    	Document agentInitTaskDoc = null;
    	try {
    		agentInitTaskDoc = AgentInitUtils.loadAgentInitTasksXML(log);
    		AgentInitUtils.addAgentTask(agentInitTaskDoc, DeviceConst.INIT_CONFIG_TASK_NAME, AgentInitUtils.AGENT_STATE_PENDING, MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName()), DeviceConst.INIT_CONFIG_TASK_NAME);
    		//AgentInitUtils.addAgentTask(agentInitTaskDoc, DeviceConst.INIT_STATUS_TASK_NAME, AgentInitUtils.AGENT_STATE_PENDING, MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName()), DeviceConst.INIT_STATUS_TASK_NAME);

    		Hashtable<Element,List<?>> listOfCommandStoBeExecuted = DeviceAppUtils.getListOfCommandsToBeExevuted(localConfigFile, log);
    		for(Element notificationEl : listOfCommandStoBeExecuted.keySet()) {
    			List<?> lc = listOfCommandStoBeExecuted.get(notificationEl);
    			String  trigger = MRXMLUtils.getValue(notificationEl, "triggername", "none");
 				String locator = MRXMLUtils.getAttributeValue(notificationEl, "parent", 
 						"relativeXpath", null);
 				
    			for (int j = 0; j < lc.size(); j++) {  
    				String taskName =  trigger;		     				
    				Element el = (Element) lc.get(j);   
    				String	locatorValue = el.getStringValue();    	
    				if (locator != null) {
    					taskName += "[" + locator + "=" +  locatorValue + "]";
     				}
    				
    				taskName = "//DeviceStatus/" + taskName;
    				AgentInitUtils.addAgentTask(agentInitTaskDoc,taskName , AgentInitUtils.AGENT_STATE_PENDING, MRRequest.getRequestName(SetServiceClientConfigRequestHandler.class.getName()), taskName);
    			}				
    		}
    	}  catch (Exception e) {
    		e.printStackTrace();
    	}
    	return agentInitTaskDoc;
    }
    
    /**
     * This method will start http(jetty) server before device login
     */
    public abstract void startHttpServer();
    
    
    /**
     * This method will be responsible for http server stop
     */
    public abstract void stopHttpServer();
   
}
