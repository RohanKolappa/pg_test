package com.ipvs.app;

import java.io.StringReader;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.XPath;
import org.dom4j.io.SAXReader;
import org.jaxen.SimpleNamespaceContext;

public class IPVSClientUtils {

	public static synchronized Document stringToDocument(String xml) throws Exception {
		if ((xml == null) || xml.trim().equals("")) {			
			throw new Exception("Error in parse, Empty xml-'" + xml + "'.");
		}

		if (!xml.trim().startsWith("<")) {
			throw new Exception("Error in parse, Invalid xml-'" + xml + "'.");
		}

		SAXReader xmlReader = null;
		try {
			xmlReader = new SAXReader();
			return xmlReader.read(new StringReader(xml));
		} finally {
			if(xmlReader !=null){                
				xmlReader=null;
			}
		}        
	}
	
	public static String documentToString(Document doc) throws Exception {        
        if (doc == null) {
            return null;
        }
        return doc.asXML();
    }

	public static String getValueXpath(Document doc, String xpath, String defaultValue) throws Exception {

		XPath xpathObj = DocumentHelper.createXPath(xpath);
		Element el = (Element)xpathObj.selectSingleNode(doc);

		if (el != null) {
			return el.getStringValue();
		}
		else
			return defaultValue;

	}

	public static Element getElementXpath(Document doc, String xpath, String xmlns) throws Exception {
		XPath xpathObj = DocumentHelper.createXPath(xpath);
		SimpleNamespaceContext context = new SimpleNamespaceContext();
		context.addNamespace("x", xmlns);
		xpathObj.setNamespaceContext(context);
		Element el = (Element)xpathObj.selectSingleNode(doc);
		return el;
	}

	public static Element getElementXpath(Document doc, String xpath) throws Exception {
		return IPVSClientUtils.getElementXpath(doc, xpath, "com.ipvs.session");
	}


	public static String getAttributeValue(Element parent, String attibutename) {
		if (parent != null) {
			Attribute attrib = parent.attribute(attibutename);
			if (attrib != null) {
				return attrib.getValue();
			}
		}
		return null;
	}

	public static String elementToString(Element parent) throws Exception {
        if (parent != null) {            
            return parent.asXML();            
        } else {
            return null;
        }
    }
	
	public static String formatResponseString(String servicename, String requestname, String userJID, String requestNID, int state, String clientdata, String data, String logData) {
        String xml = "<Response>" + "<Header serviceName='" + servicename + "' requestName='" + requestname + "'  userJID='" + userJID +
                "' requestNID='" + requestNID + "'  state='" + String.valueOf(state) + "'><ClientData>" + clientdata + "</ClientData>" + 
        		"<ClientCallback>"+IPVSClientApplet.class.getName()+"</ClientCallback>"  + "</Header>" + "<Data>" +
                data + "</Data>" + "<Log>" + logData + "</Log>" + "</Response>";
        return xml;
    }
	
	public static String generateExceptionXML(String errlog, int errcode) {
		String xml = "<Exception>" + "<Code>" + String.valueOf(errcode) + "</Code>" + "<Description>" + errlog + "</Description>" + "</Exception>";
		return xml;
	}
	
	
	
    public static String convertLatestMessagetoLegecyFormat(String response) {
    	try {
    		//System.out.println("Response:="+response);
    		Document responseDoc = DocumentHelper.parseText(response);

    		Element agentMessageElement = getElementXpath(responseDoc, "//AgentMessage");
    		Element responseElement = getElementXpath(responseDoc, "//Response");
    		if(agentMessageElement==null &&  responseElement==null) {
    			return response;
    		}

    		String agentMessageType = "";
    		Element agentMessageTypeEl =  null;
    		if(agentMessageElement !=null) {
    			agentMessageType = agentMessageElement.attributeValue("type","");
    			 agentMessageTypeEl = agentMessageElement.element(agentMessageType);
    		} else {
    			agentMessageTypeEl = responseDoc.getRootElement();
    		}

    
    		//change header
    		Element headerElement = agentMessageTypeEl.element("Header");
    		headerElement.setName("header");

    		//change data
    		Element dataElement = agentMessageTypeEl.element( "Data");
    		dataElement.setName("data");

    		//change log
    		Element logElement = agentMessageTypeEl.element("Log");
    		if(logElement!=null)
    			logElement.setName("log");

    		if(agentMessageType.equals("Event")){
    			headerElement.addElement("eventLevel").setText(headerElement.attributeValue("eventLevel", ""));
    			headerElement.addElement("eventAgentJID").setText(headerElement.attributeValue("eventAgentJID", ""));
    			headerElement.addElement("eventWallclock").setText(headerElement.attributeValue("eventWallclock", ""));
    			headerElement.addElement("eventname").setText(headerElement.attributeValue("eventname", ""));
       		} else {
    			Element clientData  = headerElement.element("ClientData");
    			//System.out.println("ClientData:="+clientData.asXML());
    			clientData.setName("clientdata");        			
    			Element clientCallback  = headerElement.element("ClientCallback");
    			clientCallback.setName("clientcallback"); 
      			
    			headerElement.addElement("requestname").setText(headerElement.attributeValue("requestName", ""));
    			headerElement.addElement("servicename").setText(headerElement.attributeValue("serviceName", ""));
    			headerElement.addElement("requestNID").setText(headerElement.attributeValue("requestNID", ""));
    			headerElement.addElement("userJID").setText(headerElement.attributeValue("userJID", ""));
    			headerElement.addElement("state").setText(headerElement.attributeValue("state", ""));        		
       		}
    		
    		System.out.println("after update:="+responseDoc.asXML());
    		
//    		//convert agentmessage structure to legacy message structure
//    		if(agentMessageElement != null) {
//    			agentMessageTypeEl.detach();
//    			Element agentMessageHeaderEl = DocumentHelper.createElement("agentmessageheader");
//    			agentMessageHeaderEl.addElement("agentJID",headerElement.attributeValue("to", ""));
//    			
//    			String agentName= "";
//    			//System.out.println("agentMessageEl:="+agentMessageTypeEl.asXML());
//    			if(agentMessageType.equals("Response")){
//    				agentName =  headerElement.elementText("clientcallback");    				
//    			} else {
//    				agentName =IPVSClientApplet.class.getName();
//    				agentMessageHeaderEl.addElement("fromagent",DocumentHelper.createXPath(".//clientcallback").selectSingleNode(agentMessageTypeEl).getText());    				
//    			}
//    			agentMessageHeaderEl.addElement("agent").setText(agentName);
//    			if(agentMessageType.equals("Event")){
//    				agentMessageType = "AgentEvent";
//    				agentMessageTypeEl.setName(agentMessageType);
//    			}
//    			
//    			agentMessageHeaderEl.addElement("agentmessagetype").setText(agentMessageType);
//    			agentMessageElement.add(agentMessageHeaderEl);
//    			
//    			
//    			Element agentMessageDataEl = DocumentHelper.createElement("agentmessagedata");  
//    			agentMessageDataEl.add(agentMessageTypeEl);
//    			agentMessageElement.add(agentMessageDataEl); 
//    			
//    			//System.out.println("convertLatestMessagetoLegecyFormat:="+responseDoc.asXML());
//    			
//    		}    		

    		return responseDoc.getRootElement().asXML();
    	} catch (Exception e) {		
    		e.printStackTrace();
    	}
    	return response;

    }
    
    
 public static String convertLegacyRequestsToLatetFormat(String  message){
    	
    	try {
    		Document messageDoc = DocumentHelper.parseText(message);
			
			Element headerElement = getElementXpath(messageDoc, "//header");
			if(headerElement ==null){
				return message;
			}
			
			Element dataElement = getElementXpath(messageDoc, "//data");
			headerElement.setName("Header");
			dataElement.setName("Data");
			headerElement.addAttribute("serviceName", headerElement.elementText("servicename"));
			headerElement.addAttribute("requestName", headerElement.elementText("requestname"));
			headerElement.addAttribute("logLevel", headerElement.elementText("loglevel"));
			
			
			Element clientDataElement = headerElement.element("clientdata");
			if(clientDataElement != null){
				clientDataElement.setName("ClientData");
			}
			
			Element clientCallbackElement = headerElement.element("clientcallback");
			if(clientCallbackElement == null){
				headerElement.addElement("ClientCallback").setText(IPVSClientApplet.class.getName());
			} else {
				clientCallbackElement.setName("ClientCallback");
			}
			
			
			
			return messageDoc.getRootElement().asXML();
		} catch (Exception e) {		
			e.printStackTrace();
		}
    	return message;
    	
    }
    



}
