package com.ipvs.client;

import java.util.Iterator;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.service.MRXMLUtils;

public final class MRClientAgentUtils {
    private static String serviceVersion="";
    private static String getServiceVersion() {
        if(serviceVersion.equals("")){
            try{
                String ver=MRXMLUtils.getServiceVersion(null);
                serviceVersion= " serviceVer='"+ver+"'";
            }catch(Exception ex){
                ex.printStackTrace();
            }
        }
        return serviceVersion;
    }

    public static String formatResponseString(String servicename, String requestname, String userJID, String requestNID, int state, String clientdata, String data, String logData,String clientcallback,boolean isDeprecated) {
        String xml = "<" + ServiceClient.RESPONSE + getServiceVersion()+">" + "<Header serviceName='" + servicename + "' requestName='" + requestname + "'  userJID='" + userJID + "' deprecated='"+ isDeprecated +
            "' requestNID='" + requestNID + "'  state='" + String.valueOf(state) + "'><ClientData>" + clientdata + "</ClientData>" + "<ClientCallback>" + clientcallback + "</ClientCallback>"  + "</Header>" + "<Data>" +
            data + "</Data>" + "<Log>" + logData + "</Log>" + "</" + ServiceClient.RESPONSE + ">";
        return xml;
    }

    public static String formatRequestString(String servicename, String requestname, String userJID, String requestNID, String clientdata,String clientcallback, String data, String deprecated) {
        String xml = "<" + ServiceClient.REQUEST + ">" + "<Header serviceName='" + servicename + "' requestName='" + requestname + "'  userJID='" + userJID + "' deprecated='"+ deprecated+
                "' requestNID='" + requestNID + "'><ClientData>" + clientdata + "</ClientData>" + "<ClientCallback>" + clientcallback + "</ClientCallback>"  + "</Header>" +
            "<Data>" + data  + "</Data>" + "</" + ServiceClient.REQUEST + ">";      
        return xml;
    }
    
    public static String createResponseXML(Document requestDoc, String userJID, String requestNID, int state, Document result, String logData) {
        String servicename = "badheader";
        String requestname = "badheader";
        String clientdata = "";
        String clientcallback = "";
        boolean isDeprecated = false;
        if (requestDoc != null) {
            try {
            	servicename = MRXMLUtils.getAttributeValueXpath(requestDoc, MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");
            	requestname = MRXMLUtils.getAttributeValueXpath(requestDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
            	clientdata = MRXMLUtils.getValueXpath(requestDoc, MRConst.HEADER_XPATH+"/"+MRConst.CLIENT_DATA, "");
            	clientcallback = MRXMLUtils.getValueXpath(requestDoc,  MRConst.HEADER_XPATH+"/"+MRConst.CLIENT_CALLBACK, "");
            	isDeprecated = Boolean.parseBoolean(MRXMLUtils.getAttributeValueXpath(requestDoc, MRConst.HEADER_XPATH,MRConst.DEPRECATED, "false"));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        String data = "";

    	if(result != null) {
	        try {
	    		data = MRXMLUtils.elementToString(result.getRootElement());
	        } catch (Exception e) {
	            data = MRException.getStackTrace(e);
	    	}
    	}
    	
        String xml = MRClientAgentUtils.formatResponseString(servicename, requestname, userJID, requestNID, state, clientdata, data, logData,clientcallback, isDeprecated);

        return xml;
    }

    public static String createErrorXML(String data, Throwable throwableObj) {
        return "<Error><data>" + data + MRException.getStackTrace(throwableObj) + "</data></Error>";
    }

    public static String createNotificationXML(String message, String type) {
        String xml = "<" + ServiceClient.NOTIFICATION + ">" + "<notificationheader>" + "<notificationtype>" + type + "</notificationtype>" + "</notificationheader>" + "<notificationdata>" + message +
            "</notificationdata>" + "</" + ServiceClient.NOTIFICATION + ">";

        try {
            Document doc = MRXMLUtils.stringToDocument(xml);
            // TBD Workaround for UI requiring fromJID to match MUC
            //String clientCallback = MRXMLUtils.getValueXpath(doc,"//"+MRConst.CLIENT_CALLBACK,"");
//            String agentName = MRXMLUtils.getValueXpath(doc,"//"+MRConst.CLIENT_CALLBACK,"");
//  
//            String action = MRXMLUtils.getValueXpath(doc, "//action","");
//            String request = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, "requestName","");
//           
//            if(DestStreamAgent.class.getName().equals(agentName) && (MediaRoomServiceConst.STARTREQUEST.equals(request))){
//       
//            	if(MediaRoomServiceConst.STARTREQUEST.equals(request)){
//            		 action = MediaRoomServiceConst.START; 
//            	}
//            	else
//            		 action = MediaRoomServiceConst.STOP;
//            	
//            	//add action element
//            	MRXMLUtils.setAttributeValueXPath(doc, MRConst.HEADER_XPATH, "requestName",MediaRoomServiceConst.ACTIONREQUEST);
//            	Element actionEle = MRXMLUtils.stringToElement("<action>"+action+"</action>");         
//            	
//            	//add streamNID element
//            	String roomNID = MRXMLUtils.getAttributeValueXpath(doc, "//StreamStatus", "roomNID","");
//            	String streamNID = MRXMLUtils.getAttributeValueXpath(doc, "//StreamStatus", "streamNID","");
//            	Element streamNIDEle = MRXMLUtils.stringToElement("<streamNID>"+streamNID+"</streamNID>");
//            	MRXMLUtils.addAttribute(streamNIDEle, "roomNID", roomNID);            	
//            
//            	Element messageElement = MRXMLUtils.getElementXpath(doc, "//message");            
//            	String streamURL = MRXMLUtils.getValueXpath(doc, "//URL", "");            
//            	String status = MRXMLUtils.getAttributeValueXpath(doc, "//Header", "state","");
//            	
//            	
//            	String fromJID = messageElement.attributeValue("from");
//            	StringTokenizer st = new StringTokenizer(fromJID, "/");
// 	            String user = st.nextToken();
// 	            messageElement.addAttribute("from", roomNID + "@conference.localhost/" + user);
// 	            Element agentMessageElement= MRXMLUtils.getElementXpath(doc, "//AgentMessage");
//	            if(agentMessageElement != null) {
//	            	agentMessageElement.addAttribute("from", roomNID + "@conference.localhost/" + user);
//	            }
//            	//change agentmessage element
//            	
//            	Element startResponse = MRXMLUtils.getElementXpath(doc, "//StartResponseData");            	
//				if(startResponse !=null)
//					startResponse.detach();
//
//				Element actionResponseData = MRXMLUtils.stringToElement("<ActionResponseData><streamURL>"+streamURL+"</streamURL><status>"+status+"</status></ActionResponseData>");
//            	
//				MRXMLUtils.addElement(actionResponseData, streamNIDEle);
//				Element relayData = MRXMLUtils.getElementXpath(doc, "//relayData");
//		            	if(relayData != null){
//		            	    Element relayDataEle = (Element)relayData.clone();
//		            	    MRXMLUtils.addElement(actionResponseData, relayDataEle);
//		            	}
//            	
//            	MRXMLUtils.addElement(actionResponseData, actionEle);
//            	MRXMLUtils.addElement(actionResponseData, "actiondata");
//            	MRXMLUtils.addElement(actionResponseData, "actionNID");
//            	Element useraction = MRXMLUtils.stringToElement("<useraction>"+action+"</useraction>");
//            	MRXMLUtils.addElement(actionResponseData, useraction);
//            
//            	MRXMLUtils.addElementXpath(doc, "//Data", actionResponseData);
//           }  else {
        	    String eventName = MRXMLUtils.getAttributeValueXpath(doc,"//Event/Header", "eventName","");
            	if("MediaPresenceEvent".equals(eventName)) {
			        	Element mediaPresenceEventData = MRXMLUtils.getElementXpath(doc, "//MediaPresenceEventData");
					String roomNID = mediaPresenceEventData.attributeValue("roomNID");
					String value = mediaPresenceEventData.attributeValue("value");
					String presenceType = mediaPresenceEventData.attributeValue("type");
					
					if("MediaRoom".equals(presenceType) && ("Unavailable".equals(value)) ) { // Generate the legacy event
						return "<Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader><notificationdata><presence to=\"\" from=\"" + roomNID + "@conference.localhost/managementserver.localhost\" type=\"unavailable\"><x xmlns=\"http://jabber.org/protocol/muc#user\"><item affiliation=\"none\" role=\"none\"><reason/><actor jid=\"\"/></item><destroy jid=\"noAlternateJID\"><reason>noReason</reason></destroy></x></presence></notificationdata></Notification>";
					}
				}
            //}

/*
	<data><MediaPresenceEventData roomNID="636b970e-03fe-47ca-a181-9cce815600d4" type="Room" value="Unavailable"/></data>

<Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader><notificationdata><presence to="sunuser3@localhost/sunpc3" from="8e4739ab-9d46-4c18-b336-8005ac135e66@conference.localhost/sunuser3@localhost" type="unavailable"><x xmlns="http://jabber.org/protocol/muc#user"><item affiliation="none" role="none"><reason/><actor jid=""/></item><destroy jid="noAlternateJID"><reason>noReason</reason></destroy></x></presence></notificationdata></Notification>
*/


            return MRXMLUtils.documentToString(doc);
        } catch (Exception e) {
        	e.printStackTrace();
            return MRClientAgentUtils.createErrorXML(xml, e);
        }
    }
    public static boolean isStale(Element root) {
        Iterator<?> i = root.elements().iterator();

        while (i.hasNext()) {
            Element x = (Element)i.next();

            if ("x".equals(x.getName())) {
                String stamp = MRXMLUtils.getAttributeValue(x,"stamp");

                if (stamp != null) {
                    return true;
                }
            }
        }

        return false;
    }
    
    
    public static String convertLegacyAgentmessageToLatetFormat(String  message,String userJID){
    	try {
    		Document messageDoc = MRXMLUtils.stringToDocument(message);
			Element agentMessageElement = MRXMLUtils.getElementXpath(messageDoc, "//AgentMessage");
			//System.out.println("convertLegacyAgentmessageToLatetFormat:="+messageDoc.asXML());
			
			if(agentMessageElement==null) {
				return message;
			}
			
			Element agentHeaderElement = MRXMLUtils.getElement(agentMessageElement, "agentmessageheader");
			MRXMLUtils.addAttribute(agentMessageElement, "from", userJID);
			MRXMLUtils.addAttribute(agentMessageElement, "to", MRXMLUtils.getValueXpath(agentHeaderElement, "./agentJID", ""));
			
			MRXMLUtils.addAttribute(agentMessageElement, "logLevel", "5");
			String toAgentName = MRXMLUtils.getValueXpath(agentHeaderElement, "./agent", "");
			
				
			//remove header
			agentHeaderElement.detach();
			
			String agentmessageType = MRXMLUtils.getValueXpath(agentHeaderElement, "./agentmessagetype", "");
			
			if(agentmessageType.equals("AgentEvent")){
				agentmessageType= ServiceClient.EVENT;
			}
			
			MRXMLUtils.addAttribute(agentMessageElement, "type", agentmessageType);
			
			//detach data node
			Element agentTypeEL  = MRXMLUtils.getElementXpath(agentMessageElement, "//"+agentmessageType);
			agentTypeEL.detach();
			agentTypeEL.setName(agentmessageType);
			
			//set new service name
			String serviceName = "";
			for(String serviceNameKey:MRClientAgent.serviceToAgentMapping.keySet()){
				if(MRClientAgent.serviceToAgentMapping.get(serviceNameKey).equals(toAgentName)){
					serviceName = serviceNameKey;
					break;
				}
			}		
			Element headerElement = MRXMLUtils.getElementXpath(agentTypeEL, ".//Header");
			MRXMLUtils.setAttributeValue(headerElement, MRConst.SERVICE_NAME, serviceName);
			
			//remove agent data node
			Element dataElement = MRXMLUtils.getElementXpath(agentMessageElement, ".//agentmessagedata");
			if(dataElement!=null)
				dataElement.detach();
			
			//add request/response/event to agent message 
			MRXMLUtils.addElement(agentMessageElement,agentTypeEL);
			
			Element clientCallBack = MRXMLUtils.getElement(headerElement, MRConst.CLIENT_CALLBACK);
			if(clientCallBack==null){
				MRXMLUtils.addElement(headerElement,MRConst.CLIENT_CALLBACK);
			}
			
			MRXMLUtils.setValueXpath(messageDoc, "//"+MRConst.CLIENT_CALLBACK, MRXMLUtils.getValueXpath(agentHeaderElement, ".//fromagent", ""));
			
			if(agentmessageType.equals(ServiceClient.RESPONSE)){
				MRXMLUtils.setValueXpath(messageDoc, "//"+MRConst.CLIENT_CALLBACK, MRXMLUtils.getValueXpath(agentHeaderElement, ".//agent", ""));
				
				
			}
			
			System.out.println("convertLegacyAgentmessageToLatetFormat:="+messageDoc.asXML());
	
			return MRXMLUtils.documentToStringOmitDeclaration(messageDoc);
		} catch (Exception e) {		
			e.printStackTrace();
		}
    	return null;
    
    }
    
    public static String convertLegacyRequestsToLatetFormat(String  message){
    	
    	try {
    		Document messageDoc = MRXMLUtils.stringToDocument(message);
			Element messageElement = messageDoc.getRootElement();
			
			Element headerElement = MRXMLUtils.getElementXpath(messageElement, ".//header");
			if(headerElement ==null){
				return message;
			}
			
			Element dataElement = MRXMLUtils.getElementXpath(messageElement, ".//data");
			headerElement.setName("Header");
			dataElement.setName("Data");
			
			MRXMLUtils.addAttribute(headerElement, MRConst.SERVICE_NAME, MRXMLUtils.getValueXpath(messageDoc, "//servicename", ""));
			MRXMLUtils.addAttribute(headerElement, MRConst.REQUEST_NAME, MRXMLUtils.getValueXpath(messageDoc, "//requestname", ""));
			MRXMLUtils.addAttribute(headerElement, MRConst.LOG_LEVEL, MRXMLUtils.getValueXpath(messageDoc, "//loglevel", ""));
			
			Element clientDataElement = MRXMLUtils.getElement(headerElement, "clientdata");
			if(clientDataElement != null){
				clientDataElement.setName(MRConst.CLIENT_DATA);
			}
			
			Element clientCallbackElement = MRXMLUtils.getElement(headerElement, "clientcallback");
			if(clientCallbackElement != null){
				clientCallbackElement.setName(MRConst.CLIENT_CALLBACK);
			}
			String state = MRXMLUtils.getValueXpath(headerElement, ".//state", "");
			if(!state.isEmpty()) {
				MRXMLUtils.addAttribute(headerElement, MRConst.STATE, MRXMLUtils.getValueXpath(messageDoc, "//state", ""));
			}
			
			return MRXMLUtils.documentToStringOmitDeclaration(messageDoc);
		} catch (Exception e) {		
			e.printStackTrace();
		}
    	return message;
    	
    }
    
    
    
    public static String convertLatestMessagetoLegecyFormat(String response) {
    	try {
    		//System.out.println("Response:="+response);
    		Document responseDoc = MRXMLUtils.stringToDocument(response);

    		Element agentMessageElement = MRXMLUtils.getElementXpath(responseDoc, "//"+ServiceClient.AGENTMESSAGE);
    		Element responseElement = MRXMLUtils.getElementXpath(responseDoc, "//"+ServiceClient.RESPONSE);
    		if(agentMessageElement==null &&  responseElement==null) {
    			return response;
    		}

    		String agentMessageType = MRXMLUtils.getAttributeValue(agentMessageElement, "type","");
    		Element agentMessageTypeEl =  null;
    		if(!agentMessageType.isEmpty()) {
    			 agentMessageTypeEl = MRXMLUtils.getElement(agentMessageElement, agentMessageType);
    		} else {
    			agentMessageTypeEl = responseDoc.getRootElement();
    		}

    
    		//change header
    		Element headerElement = MRXMLUtils.getElement(agentMessageTypeEl, "Header");
    		headerElement.setName("header");

    		//change data
    		Element dataElement = MRXMLUtils.getElement(agentMessageTypeEl, "Data");
    		dataElement.setName("data");

    		//change log
    		Element logElement = MRXMLUtils.getElement(agentMessageTypeEl, "Log");
    		if(logElement!=null)
    			logElement.setName("log");

    		if(agentMessageType.equals(ServiceClient.EVENT)){
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("eventLevel",MRXMLUtils.getAttributeValue(headerElement, "eventLevel")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("eventAgentJID",MRXMLUtils.getAttributeValue(headerElement, "eventAgentJID")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("eventWallclock",MRXMLUtils.getAttributeValue(headerElement, "eventWallclock")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("eventname",MRXMLUtils.getAttributeValue(headerElement, "eventName")));	        			
    		} else {
    			Element clientData  = MRXMLUtils.getElement(headerElement, MRConst.CLIENT_DATA);
    			clientData.setName("clientdata");        			
    			Element clientCallback  = MRXMLUtils.getElement(headerElement, MRConst.CLIENT_CALLBACK);
    			clientCallback.setName("clientcallback");        			
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("requestname",MRXMLUtils.getAttributeValue(headerElement, MRConst.REQUEST_NAME,"")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("servicename",MRXMLUtils.getAttributeValue(headerElement, MRConst.SERVICE_NAME,"")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("requestNID",MRXMLUtils.getAttributeValue(headerElement, MRConst.REQUEST_NID,"")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("userJID",MRXMLUtils.getAttributeValue(headerElement, MRConst.USER_JID,"")));
    			MRXMLUtils.addElement(headerElement,MRXMLUtils.addElement("state",MRXMLUtils.getAttributeValue(headerElement, MRConst.STATE,"")));
    		}
    		return MRXMLUtils.documentToStringOmitDeclaration(responseDoc);
    	} catch (Exception e) {		
    		e.printStackTrace();
    	}
    	return response;

    }

    public static String convertLegacyAgentmessageToLatestFormat(String  message,String userJID){
		try {

			Document messageDoc = MRXMLUtils.stringToDocument(message);
			Element agentMessageElement = MRXMLUtils.getElementXpath(messageDoc, "//AgentMessage");

			if (agentMessageElement == null) {
				return message;
			}

			Element agentHeaderElement = MRXMLUtils.getElement(agentMessageElement, "agentmessageheader");
			MRXMLUtils.addAttribute(agentMessageElement, "from", userJID);
			MRXMLUtils.addAttribute(agentMessageElement, "to", MRXMLUtils.getValueXpath(agentHeaderElement, "./agentJID", ""));

			MRXMLUtils.addAttribute(agentMessageElement, "logLevel", "5");
			// remove header
			agentHeaderElement.detach();

			String agentmessageType = MRXMLUtils.getValueXpath(agentHeaderElement, "./agentmessagetype", "");

			if (agentmessageType.equals("AgentEvent")) {
				agentmessageType = ServiceClient.EVENT;
			}

			MRXMLUtils.addAttribute(agentMessageElement, "type", agentmessageType);

			// detach data node
			Element agentTypeEL = MRXMLUtils.getElementXpath(agentMessageElement, "//" + agentmessageType);
			agentTypeEL.detach();
			agentTypeEL.setName(agentmessageType);

			Element dataEle = MRXMLUtils.getElementXpath(agentTypeEL, ".//data");
			dataEle.setName("Data");
			Element headerElement = MRXMLUtils.getElementXpath(agentTypeEL, ".//header");
			headerElement.setName("Header");
			MRXMLUtils.setAttributeValue(headerElement, MRConst.SERVICE_NAME, MRXMLUtils.getValueXpath(headerElement, "./servicename", ""));
			MRXMLUtils.setAttributeValue(headerElement, MRConst.REQUEST_NAME, MRXMLUtils.getValueXpath(headerElement, "./requestname", ""));
			MRXMLUtils.setAttributeValue(headerElement, MRConst.LOG_LEVEL, "0");
			MRXMLUtils.setAttributeValue(headerElement, MRConst.DEPRECATED, "true");
			Element clientdataEl = MRXMLUtils.getElement(headerElement, "clientdata");
			if (clientdataEl != null) {
				clientdataEl.setName(MRConst.CLIENT_DATA);
			}
			Element clientCallBack = MRXMLUtils.getElement(headerElement, MRConst.CLIENT_CALLBACK);
			if (clientCallBack == null) {
				MRXMLUtils.addElement(headerElement, MRConst.CLIENT_CALLBACK);
			}
			// remove agent data node
			Element dataElement = MRXMLUtils.getElementXpath(agentMessageElement, ".//agentmessagedata");
			if (dataElement != null)
				dataElement.detach();

			// add request/response/event to agent message
			MRXMLUtils.addElement(agentMessageElement, agentTypeEL);

			MRXMLUtils.setValueXpath(messageDoc, "//" + MRConst.CLIENT_CALLBACK, MRXMLUtils.getValueXpath(agentHeaderElement, ".//fromagent", ""));

			if (agentmessageType.equals(ServiceClient.RESPONSE)) {
				MRXMLUtils.setValueXpath(messageDoc, "//" + MRConst.CLIENT_CALLBACK, MRXMLUtils.getValueXpath(agentHeaderElement, ".//agent", ""));
			}

			return MRXMLUtils.documentToStringOmitDeclaration(messageDoc);

		} catch (Exception e) {		
			e.printStackTrace();
		}
    	return null;
    
    }
    
    public static String convertLatestAgentMessagetoLegacyFormat(String response) {
    	try {
    		Document responseDoc = MRXMLUtils.stringToDocument(response);
    		Element agentMessageElement = MRXMLUtils.getElementXpath(responseDoc, "//"+ServiceClient.AGENTMESSAGE);
    		Element responseElement = MRXMLUtils.getElementXpath(responseDoc, "//"+ServiceClient.RESPONSE);
    		if(agentMessageElement==null &&  responseElement==null) {
    			return response;
    		}
    		String agentMessageType = MRXMLUtils.getAttributeValue(agentMessageElement, "type","");
    		Element agentMessageTypeEl =  null;
    		if(!agentMessageType.isEmpty()) {
    			 agentMessageTypeEl = MRXMLUtils.getElement(agentMessageElement, agentMessageType);
    		} else {
    			agentMessageTypeEl = responseDoc.getRootElement();
    		}
    		//change header
    		Element headerElement = MRXMLUtils.getElement(agentMessageTypeEl, "Header");
    		headerElement.detach();
    		Element newHeaderEl = MRXMLUtils.newElement("header");
    		//change data
    		Element dataElement = MRXMLUtils.getElement(agentMessageTypeEl, "Data");
    		dataElement.setName("data");
    		//change log
    		Element logElement = MRXMLUtils.getElement(agentMessageTypeEl, "Log");
    		if(logElement!=null)
    			logElement.setName("log");

    		if(agentMessageType.equals(ServiceClient.EVENT)){
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("eventLevel",MRXMLUtils.getAttributeValue(headerElement, "eventLevel")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("eventAgentJID",MRXMLUtils.getAttributeValue(headerElement, "eventAgentJID")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("eventWallclock",MRXMLUtils.getAttributeValue(headerElement, "eventWallclock")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("eventname",MRXMLUtils.getAttributeValue(headerElement, "eventName")));	        			
    		} else {
    			Element clientData  = MRXMLUtils.getElement(headerElement, MRConst.CLIENT_DATA);
    			clientData.setName("clientdata");        
    			clientData.detach();
    			Element clientCallback  = MRXMLUtils.getElement(headerElement, MRConst.CLIENT_CALLBACK);
    			clientCallback.setName("clientcallback");
    			MRXMLUtils.addElement(newHeaderEl,clientData);
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("requestname",MRXMLUtils.getAttributeValue(headerElement, MRConst.REQUEST_NAME,"")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("servicename",MRXMLUtils.getAttributeValue(headerElement, MRConst.SERVICE_NAME,"")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("requestNID",MRXMLUtils.getAttributeValue(headerElement, MRConst.REQUEST_NID,"")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("userJID",MRXMLUtils.getAttributeValue(headerElement, MRConst.USER_JID,"")));
    			MRXMLUtils.addElement(newHeaderEl,MRXMLUtils.addElement("state",MRXMLUtils.getAttributeValue(headerElement, MRConst.STATE,"")));
    		}
    		MRXMLUtils.addElement(agentMessageTypeEl,newHeaderEl); 
    		//convert agentmessage structure to legacy message structure
			if (agentMessageElement != null) {
				agentMessageTypeEl.detach();
				Element agentMessageHeaderEl = MRXMLUtils.newElement("agentmessageheader");
				MRXMLUtils.addElement(agentMessageHeaderEl, MRXMLUtils.addElement("agentJID", MRXMLUtils.getAttributeValue(agentMessageElement, "to")));
				String agentName = "";
				String fromAgentName = MRClientAgent.serviceToAgentMapping.get(MRXMLUtils.getAttributeValue(headerElement, MRConst.SERVICE_NAME, ""));
				if (agentMessageType.equals(ServiceClient.RESPONSE)) {
					agentName = MRXMLUtils.getValueXpath(headerElement, ".//clientcallback", "");
				} else {
					agentName = fromAgentName;
				}
				MRXMLUtils.addElement(agentMessageHeaderEl, MRXMLUtils.addElement("fromagent", fromAgentName));
				MRXMLUtils.addElement(agentMessageHeaderEl, MRXMLUtils.addElement("agent", agentName));
				if (agentMessageType.equals(ServiceClient.EVENT)) {
					agentMessageType = "Agent" + ServiceClient.EVENT;
					agentMessageTypeEl.setName(agentMessageType);
				}
				MRXMLUtils.addElement(agentMessageHeaderEl, MRXMLUtils.addElement("agentmessagetype", agentMessageType));
				MRXMLUtils.addElement(agentMessageElement, agentMessageHeaderEl);

				Element agentMessageDataEl = MRXMLUtils.addElement("agentmessagedata");
				MRXMLUtils.addElement(agentMessageDataEl, agentMessageTypeEl);
				MRXMLUtils.addElement(agentMessageElement, agentMessageDataEl);
			}    		
    		//remove attributes
    		MRXMLUtils.getAttribute(agentMessageElement, "from").detach();
    		MRXMLUtils.getAttribute(agentMessageElement, "to").detach();
    		MRXMLUtils.getAttribute(agentMessageElement, "type").detach();
    		MRXMLUtils.getAttribute(agentMessageElement, "logLevel").detach();
    		

    		return MRXMLUtils.documentToStringOmitDeclaration(responseDoc);
    	
    		
    		} catch (Exception e) {		
    		e.printStackTrace();
    	}
    	return response;

    }

}
