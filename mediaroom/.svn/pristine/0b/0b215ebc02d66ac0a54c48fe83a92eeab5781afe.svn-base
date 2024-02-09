package com.ipvs.app;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.utils.FileUtils;

public class IPVSClientApplet extends BaseApplet implements IPVSMessageHandler {

	private static final long serialVersionUID = 1L;
    public static final String REQUEST = "Request";
    public static final String AGENTMESSAGE = "AgentMessage";
    public static final String AGENTMESSAGETYPE = "agentmessagetype";
    public static final String AGENT = "agent";
    public static final String AGENTJID = "agentJID";
    public static final String FROMAGENT = "fromagent";
    public static final String APPSERVERAGENT = "com.ipvs.mediaroomservice.impl.AppServerAgent";
    public static final String SERVICE_VERSION_FILE = "/com/ipvs/xsd/objectXml/service.version";
    public static final String VERSION = "Ver";
		
	public IPVSClient ipClient;
	
	// Error code
	public static final int XMPPLOGINFAILED = 1003;
	
	public String sendRequest_local(String userJID, String requestXML) throws Exception{
		requestXML = IPVSClientUtils.convertLegacyRequestsToLatetFormat(requestXML);
		//System.out.println("RequestAfterConversion :="+requestXML);
		Document requestDoc = IPVSClientUtils.stringToDocument(requestXML);
		Element headerElement = IPVSClientUtils.getElementXpath(requestDoc, "//Header");
		String type = "";
		if(headerElement!=null)
			type = headerElement.attributeValue("requestName");
		if("LoginRequest".equals(type)){
			String response = login_local(requestXML);		
			return response;
		}
		else if("LogoutRequest".equals(type)){
			String response = logout_local(userJID, requestXML);		
			return response;
		}
		else if("SetServiceDomainRequest".equals(type)){
			return this.setServiceDomain_local(userJID, requestXML);
		}
		else if("Request".equals(requestDoc.getRootElement().getName())){
			return this.handleRequest_local(userJID, requestXML);
		}
		else{
			return ipClient.sendPacket(requestXML);
		}
	}
	
	
	public String login_local(String loginXML) throws Exception{
        
		Document requestDoc = IPVSClientUtils.stringToDocument(loginXML);
		
		String user = IPVSClientUtils.getValueXpath(requestDoc, "//user", "");
        String resourceID = IPVSClientUtils.getValueXpath(requestDoc, "//resource", "");
        String password = IPVSClientUtils.getValueXpath(requestDoc, "//password", "");
        String serverName = IPVSClientUtils.getValueXpath(requestDoc, "//server", "");
        String portStr = IPVSClientUtils.getValueXpath(requestDoc, "//port", "5222");
        String clientdata = IPVSClientUtils.getValueXpath(requestDoc, "//ClientData", "");
        int port = Integer.parseInt(portStr);
        String servicename = IPVSClientUtils.getElementXpath(requestDoc, "//Header").attributeValue("serviceName", "badheader");
        String requestname = IPVSClientUtils.getElementXpath(requestDoc, "//Header").attributeValue("requestName", "badheader");

        String userJID = user + "@" + serverName + "/" + resourceID;
		
        String data = "";
        final String Version = "3.0.11";
        int state = 1;
        String logdata = "";
		
        try{
        	
        	this.ipClient = IPVSClient.login(serverName, port, user, password, resourceID, this);
            data = "<LoginResponseData><version>" + Version + "</version></LoginResponseData>";
            
        }catch(Exception e){
        	e.printStackTrace();
        	data = e.toString();
        	logdata = IPVSClientUtils.generateExceptionXML(e.toString(), IPVSClientApplet.XMPPLOGINFAILED);
        	state = AppletLog.ERROR;
        }

        String xml = IPVSClientUtils.formatResponseString(servicename, requestname, userJID, "", state, clientdata, data, logdata);	        
		this.handleMessage(xml);
        return xml;

	}
	
    private String setServiceDomain_local(String userJID, String request) {
    	return this.handleRequest_local(userJID, request);
    }

    private String handleRequest_local(String userJID, String request) {


    	try {
        //	String fromAgentClassName = this.getClass().getName();
       // 	String agentClassName = IPVSClientApplet.APPSERVERAGENT;
        	String messageType = IPVSClientApplet.REQUEST;
        	
        	String messageXML = 
        			 "<"+IPVSClientApplet.AGENTMESSAGE+" from='"+userJID+"' to='"+IPVSClientApplet.AGENTJID+"'" 
        	+" type='"+messageType+"' logLevel='5'>" +  request +  "</" + IPVSClientApplet.AGENTMESSAGE + ">";
//            "<" + IPVSClientApplet.AGENTMESSAGE + ">" +
//            "<agentmessageheader>" + 
//    			"<" + IPVSClientApplet.FROMAGENT + ">" + fromAgentClassName + "</" + IPVSClientApplet.FROMAGENT + ">" + 
//        		"<" + IPVSClientApplet.AGENTJID + "></" + IPVSClientApplet.AGENTJID + ">" + 
//            	"<" + IPVSClientApplet.AGENT + ">" + agentClassName + "</" + IPVSClientApplet.AGENT + ">" + 
//            	"<" + IPVSClientApplet.AGENTMESSAGETYPE + ">" + messageType + "</" + IPVSClientApplet.AGENTMESSAGETYPE + ">" + 
//            "</agentmessageheader>" + 
//            "<agentmessagedata>" + request + "</agentmessagedata>" + "</" + IPVSClientApplet.AGENTMESSAGE + ">";
        	return ipClient.sendPacket(messageXML);

        	/*
        	 * 
        	Document doc = IPVSClientUtils.stringToDocument(request);
	        String clientdata = IPVSClientUtils.getValueXpath(doc, "//clientdata", "");
	        String clientVersion = IPVSClientUtils.getValueXpath(doc, "//ServiceVersion", "");
	        String clientPrivateKey = IPVSClientUtils.getValueXpath(doc, "//AssetPrivateKeyData", "");
	        String clientTemplateTitle = IPVSClientUtils.getValueXpath(doc, "//AssetTemplateTitle", "");
			String requestXML =
					"<iq id='" + clientdata + "' type='set'>" +
					"<query xmlns='com.ipvs.session'>" +
					"<ServiceLogin clientVersion='" + clientVersion + "'>" + 
					"<clientPrivateKey>" + clientPrivateKey + "</clientPrivateKey>" +
					"<clientTemplateTitle>" + clientTemplateTitle + "</clientTemplateTitle>" +
					"</ServiceLogin>" +
					"</query>" +
					"</iq>";
			return this.sendRequest_local(userJID, requestXML);
        	 */
    	}
    	catch(Exception e) {
    		e.printStackTrace();
    		return e.toString();
    	}
    }


    public String logout_local(String userJID, String logoutXML) throws Exception{

    	Document requestDoc = IPVSClientUtils.stringToDocument(logoutXML);

    	String clientdata = IPVSClientUtils.getValueXpath(requestDoc, "//ClientData", "");
    	String servicename = IPVSClientUtils.getElementXpath(requestDoc, "//Header").attributeValue("serviceName", "badheader");
    	String requestname = IPVSClientUtils.getElementXpath(requestDoc, "//Header").attributeValue("requestName", "badheader");
    	int state = 1;
    	String data = "Logout Successful";
    	String logdata = "";

    	try{
    		IPVSClient.logout(ipClient);
    		data = "<LogoutRequestData/>";
		}catch(Exception e){
			e.printStackTrace();
			state = AppletLog.ERROR;
			logdata = e.toString();
			data = e.toString();
			
		}
		String xml = IPVSClientUtils.formatResponseString(servicename, requestname, userJID, "norequestNID", state, clientdata, data, logdata);
		this.handleMessage(xml);
		return "logout";
	}

    public String  getServiceVersion(Class c) throws Exception {
    	String serviceVersionXML = FileUtils.readFileResource(IPVSClientApplet.SERVICE_VERSION_FILE,c);    	
    	Document serviceVersionDoc = IPVSClientUtils.stringToDocument(serviceVersionXML);
    	return IPVSClientUtils.getAttributeValue(serviceVersionDoc.getRootElement(), IPVSClientApplet.VERSION); 	  
    }

	public void init_local() {
		try{
			String version = String.valueOf(this.getServiceVersion(this.getClass()));
			Document doc = IPVSClientUtils.stringToDocument("<Version>" + version + "</Version>");
			String message = IPVSClientUtils.documentToString(doc);
			callJSMethod(this.initCallback, message);
		}
		catch(Exception e){
			e.printStackTrace();
		}
	}

	
	public void handleMessage(String responseXML) {	
		try {
			callJSMethod(this.callback, IPVSClientUtils.convertLatestMessagetoLegecyFormat(responseXML));
		} catch (Exception e) {			
			e.printStackTrace();
		}
		
	}

	
	protected String handleJSCall(String methodName, String methodArg1, String methodArg2) {
		System.out.println("IPVSClientApplet:handleJSCall method=" + methodName);
		if(methodName.endsWith("init")){
    		this.init_local();
    		return "";
    	}
		
    	if(methodName.endsWith("setServiceDomain")){
    		return this.setServiceDomain_local(methodArg1, methodArg2);
    	}            

    	if(methodName.endsWith("sendRequest")){
    		try {
				return this.sendRequest_local(methodArg1, methodArg2);
			} catch (Exception e) {				
				e.printStackTrace();
			}
    	} 
    	           
    	return "";
	}

	protected void killAllThreads() {}

}
