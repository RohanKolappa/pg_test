package com.barco.utils.mr;

import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

public class ServiceAgentUtils {
	
    public static Document agentEventDoc = null;
    public static final String AGENTEVENT_XML = "/com/ipvs/mediaroomservice/impl/AgentEventInstance.xml";

	
    public static final Document getAgentEventDoc(MRLog log) throws Exception {
    	if(ServiceAgentUtils.agentEventDoc == null) {
    		ServiceAgentUtils.agentEventDoc = MRXMLUtils.loadXMLResource(
        		ServiceAgentUtils.AGENTEVENT_XML, log);
    	}
		return (Document)ServiceAgentUtils.agentEventDoc.clone();
    }

    public static String getAgentEventDataString(XMPPI xmpp, MRLog log, String roomNID,
    		String fromAgentClassName, String toAgentJID, String toAgentClassName,  
    		int eventLevel, String eventAgentJID, long eventWallclock, String eventName, 
    		Element origEventData, String logStr,
    		String info) throws Exception {
    	Document tmp = ServiceAgentUtils.getAgentEventDoc(log);
    	MRXMLUtils.getElementXpath(tmp, "//eventLevel").setText(String.valueOf(eventLevel));
    	MRXMLUtils.getElementXpath(tmp, "//eventAgentJID").setText(eventAgentJID);
    	MRXMLUtils.getElementXpath(tmp, "//eventWallclock").setText(String.valueOf(eventWallclock));
    	MRXMLUtils.getElementXpath(tmp, "//eventname").setText(eventName);
    	MRXMLUtils.getElementXpath(tmp, "//log").setText(logStr);
    	Element eventData = (Element)origEventData.clone();
    	eventData.setName(eventName + "Data");
    	MRXMLUtils.addElementXpath(tmp, "//data" ,eventData);
    	return MRXMLUtils.elementToString(tmp.getRootElement());
    }


    public static void sendAgentEvent(XMPPI xmpp, MRLog log, String roomNID,
    		String fromAgentClassName, String toAgentJID, String toAgentClassName,  
    		int eventLevel, String eventAgentJID, long eventWallclock, String eventName, 
    		Element origEventData, String logStr,
    		String info) throws Exception {
        String agentEventData = getAgentEventDataString(xmpp, log, roomNID, fromAgentClassName, toAgentJID, toAgentClassName, eventLevel, eventAgentJID, eventWallclock, eventName, origEventData, logStr, info);
        ServiceAgentUtils.sendAgentMessage(xmpp, log, fromAgentClassName, roomNID, toAgentJID, toAgentClassName, ServiceClient.AGENTEVENT, agentEventData, info);
    }

    public static final String getAgentName(String name, String agentJID) {
    	return name;
    }
    
    public static final String getAgentName(String agentJID) {
         StringTokenizer st = new StringTokenizer(agentJID, "/");
       String agentName = null;
       while(st.hasMoreTokens()){
    	   agentName = st.nextToken();
       }
    	
       return agentName;
    }

    public static String getAgentMessage(String fromAgentClassName, String roomNID, 
    		String agentJID, String agentClassName, String messageType, String data, String info) {
       return  "<x xmlns='" + IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS + "'>" + 
        "<" + ServiceClient.AGENTMESSAGE + ">" +
        "<agentmessageheader>" + 
			"<" + ServiceClient.FROMAGENT + ">" + fromAgentClassName + "</" + ServiceClient.FROMAGENT + ">" + 
    		"<" + ServiceClient.AGENTJID + ">" + agentJID + "</" + ServiceClient.AGENTJID + ">" + 
        	"<" + ServiceClient.AGENT + ">" + getAgentName(agentClassName, agentJID) + "</" + ServiceClient.AGENT + ">" + 
        	"<" + ServiceClient.AGENTMESSAGETYPE + ">" + messageType + "</" + ServiceClient.AGENTMESSAGETYPE + ">" + 
        "</agentmessageheader>" + 
        "<agentmessagedata>" + data + "</agentmessagedata>" + "</" + ServiceClient.AGENTMESSAGE + ">" +
        "</x>";
    }    
   

    public static final void sendAgentMessage(XMPPI xmpp,MRLog log, 
    		String fromAgentClassName, String roomNID, 
    		String agentJID, String agentClassName, String messageType, String data, String info)
        throws MRException {
        log.addInfo(info + ":" + agentJID, MRLog.OK, MRLog.NOTIFY);
        try {        	
     		String message = getAgentMessage(fromAgentClassName, roomNID, agentJID, agentClassName, messageType, data, info);
            xmpp.sendMessage(agentJID, message,IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);
        }catch(MRException exp){        	
          throw exp;	
        } catch (Exception e) {
            e.printStackTrace();
            throw new MRException(MRExceptionCodes.SYSTEMERROR, "<error>" + "Cannot send Agent message" + "</error>");
        }
    }

}
