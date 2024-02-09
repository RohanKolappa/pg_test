package com.barco.utils.mr;

import java.util.Iterator;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.systemservice.handler.SetServiceDomainRequestHandler;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.smack.IPVSLLSessionI;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public final class ServiceAgentUtils1 {
    public static final String XMLNSMUC = "http://jabber.org/protocol/muc#user";
    public static final String XMLNSPUBSUB = "http://jabber.org/protocol/pubsub#event";
    public static final String MUCUNAVAILABLE = "unavailable";
    public static final String GROUPCHAT = "groupchat";
    public static final String PRESENCE = "presence";
    public static final String MESSAGE = "message";
    private static final String INSTANCE_LIST_NID = "DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList";
    public static final String EVENT = "Event";
    public static final String AGENTEVENT_XML = "/com/ipvs/mediaroomservice/impl/AgentEventInstance.xml";
    public static Document agentEventDoc = null;

    public static final Document getAgentEventDoc(MRLog log) throws Exception {
    	if(ServiceAgentUtils.agentEventDoc == null) {
    		ServiceAgentUtils.agentEventDoc = MRXMLUtils.loadXMLResource(
        		ServiceAgentUtils.AGENTEVENT_XML, log);
    	}
		return (Document)ServiceAgentUtils.agentEventDoc.clone();
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
    
    public static final String getBareJID(String userJID){
        if(userJID.indexOf("/")!=-1) {
            StringTokenizer stkresource = new StringTokenizer(userJID, "/");
            return stkresource.nextToken();  
        }
        return userJID;
    }
    
    private static String getRequestRoot(String requestName) {
        if (requestName.contains(ServiceClient.REQUEST)) {
            return requestName.replace(ServiceClient.REQUEST, "");
        } else if (requestName.contains(ServiceClient.RESPONSE)) {
            return requestName.replace(ServiceClient.RESPONSE, "");
        }

        return requestName;
    }

    public static Element cloneDataElement(Document doc) throws Exception {
        String agentMessageType = MRXMLUtils.getValueXpath(doc, "//" + ServiceClient.AGENTMESSAGETYPE,"");
        Element rqdata = null;

        if (ServiceClient.REQUEST.equals(agentMessageType)) {
            String requestName = MRXMLUtils.getValueXpath(doc, "//requestname","");
            rqdata = MRXMLUtils.getElementXpath(doc, "//" + ServiceAgentUtils.getRequestRoot(requestName) + "RequestData");
        } else if (ServiceClient.RESPONSE.equals(agentMessageType)) {
            String requestName = MRXMLUtils.getValueXpath(doc, "//requestname","");
            rqdata = MRXMLUtils.getElementXpath(doc, "//" + ServiceAgentUtils.getRequestRoot(requestName) + "ResponseData");
        } else {
            rqdata = MRXMLUtils.getElementXpath(doc, "//data");
        }

        if (rqdata == null) {
            System.out.println("agentmessagetype=" + agentMessageType + MRXMLUtils.documentToString(doc));
        }
        if(rqdata==null){
        	return null;
        }
        Element data = ((Element) rqdata.clone());
        data.setName("data");

        return data;
    }

    // TBD : Get rid of this call and have all calling methods pass the request
    public static final void sendAgentRequestMessage(XMPPI xmpp, MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String requestName, String data, String info, String requestNID)
        throws Exception {
        String requestRoot = ServiceAgentUtils.getRequestRoot(requestName);
        data = data.replaceAll("<data>", "<" + requestRoot + "RequestData>");
        data = data.replaceAll("</data>", "</" + requestRoot + "RequestData>");
        data = MRClientAgentUtils.formatRequestString("none", requestName, "none", requestNID, "none", data);
        ServiceAgentUtils.sendAgentMessage(xmpp, log, "", roomNID, agentJID, agentClassName, ServiceClient.REQUEST, data, info);
    }

    
    public static final void sendAgentRequestMessage(XMPPI xmpp, MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String requestName, String data, String info, String requestNID, String clientData)
        throws Exception {
        String requestRoot = ServiceAgentUtils.getRequestRoot(requestName);
        data = data.replaceAll("<data>", "<" + requestRoot + "RequestData>");
        data = data.replaceAll("</data>", "</" + requestRoot + "RequestData>");
        data = MRClientAgentUtils.formatRequestString("none", requestName, "none", requestNID, clientData, data);
        ServiceAgentUtils.sendAgentMessage(xmpp, log, "", roomNID, agentJID, agentClassName, ServiceClient.REQUEST, data, info);
    }

    public static final void sendAgentRequestMessage(XMPPI xmpp, MRLog log, String fromAgentClassName, String roomNID, 
    		String agentJID, String agentClassName, Document requestDoc, String info, String requestNID)
    	throws Exception {
        String data = MRXMLUtils.elementToString(requestDoc.getRootElement());
        ServiceAgentUtils.sendAgentMessage(xmpp, log, fromAgentClassName, roomNID, agentJID, agentClassName, ServiceClient.REQUEST, data, info);    	
    }
    
    public static final void sendAgentRequestMessage(XMPPI xmpp, MRLog log, String roomNID, String fromAgentClassName,
    		String agentJID, String toAgentClassName, String requestName, String data, String info, String requestNID,String userJID,String serviceName,String clientData)
        throws Exception {
        String requestRoot = ServiceAgentUtils.getRequestRoot(requestName);
        data = data.replaceAll("<data>", "<" + requestRoot + "RequestData>");
        data = data.replaceAll("</data>", "</" + requestRoot + "RequestData>");
        data = MRClientAgentUtils.formatRequestString(serviceName, requestName, userJID, requestNID, clientData, data);
        ServiceAgentUtils.sendAgentMessage(xmpp, log, fromAgentClassName, roomNID, agentJID, toAgentClassName, ServiceClient.REQUEST, data, info);
    }


    public static void sendAgentResponse(XMPPI xmpp, MRLog log, MRClient client, Document doc, String toJID, String response,
    		String fromAgentJID, String fromAgentClassName, int responseState)
        throws Exception {
    	String fromAgentName = MRXMLUtils.getValueXpath(doc, "//" + ServiceClient.FROMAGENT, "");
        // System.out.println("SourceTable:sendClientMessage fromAgentName=" + fromAgentName + " doc=" + MRXMLUtils.documentToString(doc));
        String requestNID = MRXMLUtils.getValueXpath(doc, "//requestNID", "");
        String serviceName = MRXMLUtils.getValueXpath(doc, "//servicename", "");
        String requestName = MRXMLUtils.getValueXpath(doc, "//requestname", "");
        String clientData = MRXMLUtils.getValueXpath(doc, "//clientdata", "");
        String roomNID = null;
        String toAgentJID = toJID;
        String toAgentClassName = fromAgentName;
        String data = response;
        String info = fromAgentClassName + ":" + fromAgentJID + " > streamInfo Response  to " + ServiceAgentUtils.getAgentShortName(fromAgentName);
        ServiceAgentUtils.sendAgentResponseMessage(xmpp, log, roomNID, fromAgentClassName, 
        		toAgentJID, toAgentClassName, serviceName, requestName, data, info, responseState, "", requestNID, fromAgentJID, clientData);
    }
    
    
    
    public static final void sendAgentResponseMessage(XMPPI xmpp,IPVSLLSessionI linkLocalSession, MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String requestName, String data, String info, int responseState,
            String logdata, Document request) throws Exception {
    	String userJID = MRXMLUtils.getValueXpath(request, "//userJID", "none");
    	String requestNID = MRXMLUtils.getValueXpath(request, "//requestNID", "none");
    	String clientdata = MRXMLUtils.getValueXpath(request, "//clientdata", "none");
    	String serviceName = MRXMLUtils.getValueXpath(request, "//servicename", "none");
    	ServiceAgentUtils.sendAgentResponseMessage(xmpp, linkLocalSession, log, roomNID, 
    			agentJID, agentClassName, serviceName, requestName, data, info, responseState, logdata, requestNID, userJID, clientdata);
    }

    public static final void sendAgentResponseMessage(XMPPI xmpp, MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String requestName, String data, String info, int responseState,
            String logdata, Document request) throws Exception {
    	sendAgentResponseMessage(xmpp, null, log, roomNID, agentJID, agentClassName, requestName, data, info, responseState, logdata, request);
    }

    // TBD : Get rid of this call and have all calling methods pass the request
    public static final void sendAgentResponseMessage(XMPPI xmpp, MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String requestName, String data, String info, int responseState,
            String logdata, String requestNID) throws Exception {
    	ServiceAgentUtils.sendAgentResponseMessage(xmpp, null,log, roomNID, 
    			agentJID, agentClassName, "none", requestName, data, info, responseState, logdata, requestNID, "none", "none");
    }

    private static final void sendAgentResponseMessage(XMPPI xmpp,IPVSLLSessionI linkLocalSession,MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String serviceName, String requestName, String data, String info, int responseState,
        String logdata, String requestNID, String userJID, String clientdata) throws Exception {
        String requestRoot = ServiceAgentUtils.getRequestRoot(requestName);
        data = data.replaceAll("<data>", "<" + requestRoot + "ResponseData>");
        data = data.replaceAll("</data>", "</" + requestRoot + "ResponseData>");
        data = MRClientAgentUtils.formatResponseString(serviceName, requestRoot + ServiceClient.REQUEST, 
        		userJID, requestNID, responseState, clientdata, data, logdata);
        ServiceAgentUtils.sendAgentMessage(xmpp,linkLocalSession,log, "", roomNID, agentJID, agentClassName, ServiceClient.RESPONSE, data, info);
    }
    
    public static final void sendAgentResponseMessage(XMPPI xmpp, MRLog log, String roomNID, String fromAgentClassName,
    		String toAgentJID, String toAgentClassName, String serviceName, String requestName, String data, String info, int responseState,
        String logdata, String requestNID, String userJID, String clientdata) throws Exception {
        String requestRoot = ServiceAgentUtils.getRequestRoot(requestName);
        data = data.replaceAll("<data>", "<" + requestRoot + "ResponseData>");
        data = data.replaceAll("</data>", "</" + requestRoot + "ResponseData>");
        data = MRClientAgentUtils.formatResponseString(serviceName, requestRoot + ServiceClient.REQUEST, 
        		userJID, requestNID, responseState, clientdata, data, logdata);
        ServiceAgentUtils.sendAgentMessage(xmpp, log, fromAgentClassName, roomNID, toAgentJID, toAgentClassName, ServiceClient.RESPONSE, data, info);
    }

    public static final void sendAgentEventMessage(XMPPI xmpp, MRLog log, String roomNID, 
    		String agentJID, String agentClassName, String eventName, String data, String info)
        throws Exception {
        data = "<AgentEvent><header/>" + data + "</AgentEvent>";
        ServiceAgentUtils.sendAgentMessage(xmpp, log, "", roomNID, agentJID, agentClassName, ServiceClient.AGENTEVENT, data, info);
    }
    
    
    public static final void sendAgentMessage(XMPPI xmpp, MRLog log, 
    		String fromAgentClassName, String roomNID, 
    		String agentJID, String agentClassName, String messageType, String data, String info)
        throws MRException {
    	sendAgentMessage(xmpp, null, log, fromAgentClassName, roomNID, agentJID, agentClassName, messageType, data, info);
    }

    public static final void sendAgentMessage(XMPPI xmpp,IPVSLLSessionI linkLocalSession, MRLog log, 
    		String fromAgentClassName, String roomNID, 
    		String agentJID, String agentClassName, String messageType, String data, String info)
        throws MRException {
        log.addInfo(info + ":" + agentJID, MRLog.OK, MRLog.NOTIFY);
        try {        	
            if (roomNID != null) {
            	String message = getAgentMessage(fromAgentClassName, roomNID, agentJID, agentClassName, messageType, data, info);
                    
                xmpp.sendRoomMessage(roomNID, message,
                		IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);

            } else {
         		String message = getAgentMessage(fromAgentClassName, roomNID, agentJID, agentClassName, messageType, data, info);                    
                
            	if(MRClientUtils.isLinkLocalJID(agentJID) && linkLocalSession !=null) {
            		linkLocalSession.sendLLMessage(agentJID, message,IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);
            	} else {                
                    xmpp.sendMessage(agentJID, message,IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);
            	}
            }
        }catch(MRException exp){        	
          throw exp;	
        } catch (Exception e) {
            e.printStackTrace();
            throw new MRException(MRExceptionCodes.SYSTEMERROR, "<error>" + "Cannot send Agent message" + "</error>");
        }
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
   


    /*
    public void joinRooms(XMPPI xmpp, MRLog log, MRDB db, String userJID) throws Exception {
        Document rdoc = MRXMLUtils.stringToDocument("<joinrooms/>");
        String requestNID = "requestNID";
        MRRequest request = new MRRequest(userJID, MRLog.INFO, requestNID, "serviceName", "requestName", new MRMessage(rdoc), "");
        int index = userJID.indexOf("/");
        String agentJID = userJID.substring(0, index);
        String xpath = "//Stream[data[sourceAgentJID=\"" + agentJID + "\"]]";
    
        // The roomNIDList should be obtained from MUC
        // List<String> roomNIDList = db.elements(request, db.getRoomRootNID());
        // Iterator<String> i = roomNIDList.iterator();
    
        Element roomNIDListDoc = MRXMLDBUtils.findChildElements(1, -1, db.getRoomRootNID(),
                        null, null, 1, null, xpath, request, db, log);
    
    
        while (i.hasNext()) {
            String roomNID = i.next();
            log.addInfo("SourceStreamAgent: checkStreamList roomNID=" + roomNID);
    
            Document roomdoc = MRXMLDBUtils.readElement(request, log, db, roomNID);
            List<?> streams = MRXMLUtils.getListXpath(roomdoc, xpath);
            Iterator<?> si = streams.iterator();
            String name = MediaRoomServiceConst.DESTSTREAMAGENT;
    
            while (si.hasNext()) {
                Element stream = (Element) (si.next());
                Element destAgentJIDElement = MRXMLUtils.getElementXpath(stream, "//destAgentJID");
                String streamNID = stream.getAttributeValue("NID");
    
                        String info = "SourceStreamAgent: sending presence streamNID=" + streamNID + " from " +
                        MediaRoomServiceConst.SOURCESTREAMAGENT + " to " + name;
                        ServiceAgentUtils.sendAgentMessage(xmpp, log,
                            roomNID, destAgentJIDElement.getValue(),
                            MediaRoomServiceConst.DESTSTREAMAGENT,
                            "<data><action>" + MediaRoomServiceConst.PRESENCE + "</action>" +
                          "<streamNID>" + streamNID + "</streamNID></data>", info);
            }
        }
    }
    */
    public static String getRoomNickname(String userJID) {
        StringTokenizer st = new StringTokenizer(userJID, "/");
        if(st.hasMoreTokens())
        	return st.nextToken();
        else
        	return null;
    }

    public static String getRoomUserJID(String nicknameJID) {
        StringTokenizer st = new StringTokenizer(nicknameJID, "/");
        st.nextToken();

        return st.nextToken();
    }

    public static String getRoomNID(String nicknameJID) {
        StringTokenizer st = new StringTokenizer(nicknameJID, "@");

        return st.nextToken();
    }

    public static boolean roomUnvailable(Element event, boolean available) throws Exception {
	    if (available) {
	        return false;
	    }
	
	    Iterator<?> i = event.elements().iterator();
	
	    while (i.hasNext()) {
	        Element x = (Element) i.next();
	
	        if ("x".equals(x.getName())) {
	            Iterator<?> j = x.elements().iterator();
	
	            while (j.hasNext()) {
	                Element destroy = (Element) j.next();
	
	                if ("destroy".equals(destroy.getName())) {
	                    return true;
	                }
	            }
	        }
	    }
	
	    return false;
	}


    public static boolean isAvailable(XMPPI xmpp, String roomNID, String userJID, String myJID) throws MRException,Exception {
        // TBD : Temp fix if myJID is not owner xmpp.isAvailable fails
        XMPPTransaction.startBlock("isAvailable");
    	boolean result = false;
        try {
        	if(myJID.equals(userJID))
        		result = true;
        	String nickname = ServiceAgentUtils.getRoomNickname(userJID);
        	if(nickname != null)
        		result = xmpp.isAvailable(roomNID, nickname);
        } catch (Exception e) {
        	e.printStackTrace();
        	throw new MRException(MRExceptionCodes.CHECKING_AVAILABLE_PRESENCE_FAILED,"Checking Available Presence Failed.UserJID:="+userJID );
            //result = false;
        }
        XMPPTransaction.endBlock("isAvailable");
        return result;
    }

    public static String getUserFromJID(String userJID) {
    	return userJID;
    }

    public static boolean matchesJID(String jid1, String jid2) {
    	if(jid1==null || jid2 == null) {
    		return false;
    	}
    	
    	if(MediaRoomServiceConst.WILDCARD.equals(jid2))
    		return true;
    	
    	return jid1.equals(jid2);    	
    }


    public static String getAgentShortName(String agentName) {
        StringTokenizer st = new StringTokenizer(agentName, ".");
        String shortName = "";

        while (st.hasMoreTokens())
            shortName = st.nextToken();

        return shortName;
    }

    public static String formatNotify(String rootName, String streamNID, String actionNID, String notifydata, int status) {
        return "<data>" + 
        "<action>" + rootName + "</action>" + 
        "<streamNID>" + streamNID + "</streamNID>" + 
        "<actionNID>" + actionNID + "</actionNID>" + 
        "<notifydata>" + notifydata + "</notifydata>" + 
        "<status>" + String.valueOf(status) + "</status>" + 
        "</data>";
    }

    public static String getResourceTitle(String userJID) {
        String resource = null;
        StringTokenizer resourceStk = new StringTokenizer(userJID, "/");

        while (resourceStk.hasMoreTokens()) {
            resource = resourceStk.nextToken();
        }

        return resource;
    }

    public static String getDeviceNID(String userJID, MRDB db, MRLog log) throws MRException,Exception{
    	String deviceNID = null;


    	String resourceTitle = getResourceTitle(userJID);

    	if ((resourceTitle == null) || (resourceTitle.length() == 0) || userJID.startsWith("superuser-")) {
    		return null;
    	}

    	MRRequest request = MRRequest.createRequest(userJID);
    	Element assetResourceElement = DBServiceUtils.getAssetResource(resourceTitle, request, log, db);

    	if(assetResourceElement==null){
    		return null;
    	}

    	deviceNID = getDeviceNID(MRXMLUtils.elementToDocument(assetResourceElement), userJID, db, log);      

    	return deviceNID;
    }
    
    public static String getDeviceNID(String assetResourceNID, String userJID, MRDB db, MRLog log) throws MRException,Exception{
    	Document assetResourceDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, assetResourceNID);
    	return getDeviceNID(assetResourceDoc, userJID, db, log);
    }

    public static String getDeviceNID(Document assetResourceDoc, String userJID, MRDB db, MRLog log) throws MRException,Exception {

    	Document deviceDoc = getDeviceDoc(assetResourceDoc, userJID, db, log);

    	if (deviceDoc == null) {
    		return null;
    	}

    	return MRXMLUtils.getAttributeValue(deviceDoc.getRootElement(), "NID", "");

    }
    
    public static String getMediaStreamRelayPortResourceNID(String assetResourceNID, String userJID, MRDB db, MRLog log,XMPPI xmpp) throws MRException,Exception {
    	
    	if(assetResourceNID.isEmpty()) {
    		return null;
    	}
    	
    	MRRequest request = MRRequest.createRequest(userJID);

    	Document  deviceDoc = AssetAdminServiceUtils.getDeviceResourcePointers(assetResourceNID, request, db, log, xmpp);    
    
        String mediaStreamRelayPortResourceNID = MRXMLUtils.getValueXpath(deviceDoc, "//MediaStreamRelayPortResourceNID/NIDValue", "");
         
    	return mediaStreamRelayPortResourceNID;
    }
    
    public static Document getDeviceDoc(String assetResourceNID, String userJID, MRDB db, MRLog log) throws MRException,Exception{
     	Document assetResourceDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, assetResourceNID);
    	return getDeviceDoc(assetResourceDoc, userJID, db, log);
    }
    
    
    
    public static Document getDeviceDoc(Document assetResourceDoc, String userJID, MRDB db, MRLog log) throws MRException,Exception{
    	String assetResounceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), "NID", "");
    	if(assetResounceNID.isEmpty()) {
    		return null;
    	}
    	MRRequest request = MRRequest.createRequest(userJID);
    	//String deviceType = MRXMLUtils.getValueXpath(assetResourceDoc, "//Type", "");
    	MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.Device, log);
    	String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

    	String whereXPath = "AssetResourceNID='" + assetResounceNID + "'";

    	config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.Device, log);
    	parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

    	Element response = MRXMLUtils.newElement("Device");

    	String[] xpathArr = { whereXPath };   	    	
    	MRXMLDBUtils.findChildElements(parentNID, response, null, 0, null, xpathArr, request, db, log);
    	Element deviceEl = MRXMLUtils.getElementXpath(response,"./*");    	
    	if(deviceEl == null) {
    		
    		return null;
    	}
    	return MRXMLUtils.elementToDocument(deviceEl);
    }

    public static String getAssetNID(String userJID, MRDB db, MRLog log) {
        String assetResourceNID = null;

        try {
            String resourceTitle = getResourceTitle(userJID);

            if ((resourceTitle == null) || (resourceTitle.length() == 0) || userJID.startsWith("superuser-")) {
                return null;
            }

            MRRequest request = MRRequest.createRequest(userJID);
            assetResourceNID = DBServiceUtils.getAssetResourceNID(resourceTitle, request, log, db);
        } catch (Exception e) {
            return null;

            //ignore the error
        }

        return assetResourceNID;
    }
    
    public static Element getAssetDoc(String userJID, MRDB db, MRLog log) {
        Element assetResourceEL = null;

        try {
            String resourceTitle = getResourceTitle(userJID);

            if ((resourceTitle == null) || (resourceTitle.length() == 0) || userJID.startsWith("superuser-")) {
                return null;
            }

            MRRequest request = MRRequest.createRequest(userJID);
            assetResourceEL = DBServiceUtils.getAssetResource(resourceTitle, request, log, db);
        } catch (Exception e) {
            return null;

            //ignore the error
        }

        return assetResourceEL;
    }
    
    
    public static String getMediaFileRootNID(String userJID, MRDB db, MRLog log) {
       
        String mediaStreamFileRootNID =null;
        try {
          
            MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
             mediaStreamFileRootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
            
        } catch (Exception e) {
            return null;          
        }

        return mediaStreamFileRootNID;
    }

    public static String getDeviceInstaceNID() {
        return INSTANCE_LIST_NID;
    }

    //execute the CLI command
    public static Document executeCLICommand(String[] cmdArray, MRLog log) {
        String cmdresult = null;

        try {
            cmdresult = MediaRoomServiceUtils.executeCLICommand(cmdArray);

            //System.out.println("cmdresult--->" + cmdresult);
            int index = cmdresult.indexOf("<");

            if (index != -1) {
                cmdresult = cmdresult.substring(index).trim();
            }
        } catch (Exception e) {
            log.addInfo("Exception occur when executing the CLI command in executeCLICommand() method  : \n" + e.getMessage());
        }

        if ((cmdresult != null) && (cmdresult.length() != 0)) {
            try {
                Document cmdResultdoc = MRXMLUtils.stringToDocument(cmdresult);

                String status = MRXMLUtils.getValueXpath(cmdResultdoc, "//Command/Status", "");

                if (status.equals("Success")) {
                    return cmdResultdoc;
                } else {
                    return null;
                }
            } catch (Exception e) {
                log.addInfo("Exception occur when executing the ELI command in executeCLICommand() method  : \n" + e.getMessage());
                e.printStackTrace();
            }
        }

        return null;
    }  
    
  //Create Media Store Event
    public static String createAgentStateEventsMessage(String action,String agentClass,String agentState) {
        return "<data> " + "<action>" + action + "</action> <AgentName>"+agentClass+"</AgentName>" +
        		"<AgentState>"+agentState+"</AgentState></data>";
    }

    //Send Message to source Stream Agent
    public static void sendAgentStateEvent(String userJID, String action, String toAgentClassName,String fromClassName,String agentState, XMPPI xmpp, MRLog log)
        throws MRException, Exception {
        String info = ServiceAgentUtils.getAgentShortName(fromClassName) + " > action="+action + " to " + ServiceAgentUtils.getAgentShortName(toAgentClassName);
        String request = createAgentStateEventsMessage(action,fromClassName,agentState);
        ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, userJID, toAgentClassName, action, request, info);
    }
    
    
    //Create Media Store Event
    public static String createAgentResourceStateEventsMessage(String action,String agentClass,String resourceNID,String resourceType,String resourceState) {
        return "<data> " + "<action>" + action + "</action> <AgentName>"+agentClass+"</AgentName>" +
        		"<AgentResourceNID>"+resourceNID+"</AgentResourceNID><AgentResourceType>"+resourceType+"</AgentResourceType>" +
        				"<AgentResourceState>"+resourceState+"</AgentResourceState></data>";
    }
    
    //Send Message to source Stream Agent
    public static void sendAgentResourceStateEvent(String userJID, String action, String toAgentClassName,String fromClassName, String resourceNID,String resourceType,String resourceState, XMPPI xmpp, MRLog log)
        throws MRException, Exception {
        String info = ServiceAgentUtils.getAgentShortName(fromClassName) + " > action="+action + " to " + ServiceAgentUtils.getAgentShortName(toAgentClassName);
        String request = createAgentResourceStateEventsMessage(action,fromClassName,  resourceNID, resourceType,resourceState);
        ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, userJID, toAgentClassName, action, request, info);
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
    
    public static boolean isShutdownEvent(String message) {
    	try {
    		Document messageDoc = MRXMLUtils.stringToDocument(message);
    		Element shutdownEL = MRXMLUtils.getElementXpath(messageDoc, "//shutdown");    		
    		if(shutdownEL!= null) {
    		 return true;	
    		}
    	} catch (Exception e) {
    	
    	}
    	return false;  	
    }
    
    
    public static boolean isSetServiceDomainFailed(String message,MRLog log) {
    	try {
    		int exceptionCode = getSetServiceDomainState(message, log);
    		if(exceptionCode != MRRequest.STATE_DONE) {
    			return true;
    		}
    	} catch (Exception e) {    	
    	}
    	return false;  	
    }

    public static int getSetServiceDomainState(String message,MRLog log) throws MRException,Exception {
    	Document messageDoc = MRXMLUtils.stringToDocument(message);    		 
    	String agentmessagetype = MRXMLUtils.getValueXpath(messageDoc, "//agentmessagetype", "");
    	String requestName = MRXMLUtils.getValueXpath(messageDoc, "//requestname", "");

    	if (agentmessagetype.equals(ServiceClient.RESPONSE) && requestName.equals(MRRequest.getRequestName(SetServiceDomainRequestHandler.class.getName()))) {
    		int state = Utils.getIntValue(MRXMLUtils.getValueXpath(messageDoc, "//state",""), MRRequest.STATE_EXCEPTION);
    		if(state == MRRequest.STATE_EXCEPTION){
    			log.addInfo("SetServiceDomainFailed", MRLog.INFO,MRLog.NOTIFY);
    			System.out.println(message);
    			int exceptionCode = Utils.getIntValue(MRXMLUtils.getValueXpath(messageDoc, "//Exception/Code", ""), 200);
    			return exceptionCode;
    		}    		     	
    	}

    	return MRRequest.STATE_DONE;
    }
    
    
    public static String getDeviceType(Document assetResourceDoc) throws MRException,Exception {
    	if(assetResourceDoc ==null) {
    		return null;
    	}
    	return MRXMLUtils.getValueXpath(assetResourceDoc, "//" + AMConst.AssetResource + "/" + AMConst.INFO + "/" + AMConst.TYPE, null); 
    }
}
