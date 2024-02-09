package com.ipvs.sessionservice.impl;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.XPath;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.notification.BaseNotifier;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;


public class SessionServiceUtils {
    public static final String BOOKMARK_RESPONSE_XML = "/com/ipvs/sessionservice/xml/BookMarkResponse.xml";
    public static final String SERVICELOGIN_RESPONSE_XML = "/com/ipvs/sessionservice/xml/ServiceLoginResponse.xml";
    public static final String ADDBOOKMARK_ACTION_XML = "/com/ipvs/mediaroomservice/xml/AddBookmarkRequest.xml";
    public static final String GETBOOKMARK_LIST_XML = "/com/ipvs/mediaroomservice/xml/GetBookmarkListRequest.xml";
    public static final String DELETEBOOKMARK_XML = "/com/ipvs/mediaroomservice/xml/DeleteBookmarkRequest.xml";
    public static final String TIMELINE_INSTANCE_XML = "/com/ipvs/sessionservice/impl/TimelineInstance.xml";
    public static final String SESSION_INSTANCE_XML = "/com/ipvs/sessionservice/impl/SessionInstance.xml";
    public static final String CONNSTATE_INSTANCE_XML = "/com/ipvs/sessionservice/impl/ConnStateInstance.xml";
    public static final String ENTITYPRESENCE_INSTANCE_XML = "/com/ipvs/sessionservice/impl/EntityPresenceInstance.xml";
    public static final String SESSION_REQUEST_TO_SERVICE_NAME_MAPPING = "/com/ipvs/sessionservice/impl/SessionRequestToServiceNameMapping.xml";
    private static Document sessionRequesttoServiceMappingDoc = null;
    private static Document requestDocTemplate = null;

    public static String sessionServiceRequestToIQXml(Document doc, String from, String to) throws Exception {
        // System.out.println("Converting sessionservice request to IQ");
        String clientData = MRXMLUtils.getValueXpath(doc, "//"+MRConst.CLIENT_DATA, "");
        String requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH , MRConst.REQUEST_NAME, "");
        clientData = requestName + "." + clientData;

        String dataElementName = requestName.substring(0, requestName.length() - ServiceClient.REQUEST.length());
        Element dataElement = MRXMLUtils.getElementXpath(doc, "//" + dataElementName);
        
        // TBD: Remove Temp handling of deprecated
        dataElement.setName(requestName);
        //System.out.println("SessionServiceUtils:sessionServiceRequestToIQXml sending=" + MRXMLUtils.elementToString(dataElement));

        return "<iq from='" + from + "' id='" + clientData + "' to='" + to + "' type='set'>" + "<query xmlns='" +
        IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" + MRXMLUtils.elementToString(dataElement) + "</query>" + "</iq>";
    }

    public static Document QueryToSessionServiceRequestDoc(String fromJID, Element query, String id, MRLog log) throws Exception {
        String queryXML = MRXMLUtils.elementToString(query);
        int beginIndex = queryXML.indexOf("<", 1);
        int endIndex = queryXML.lastIndexOf("</query>");
        String dataXML = queryXML.substring(beginIndex, endIndex);
        Element dataElement = MRXMLUtils.stringToElement(dataXML);
        String requestName = dataElement.getName();

        // Check if deprecated
        if(!requestName.endsWith(ServiceClient.REQUEST)) {
        	requestName = requestName + ServiceClient.REQUEST;
        	dataElement.addAttribute("deprecated", "true");
        }
        
    	String requestRootName = requestName.substring(0, requestName.length() - ServiceClient.REQUEST.length());
        dataElement.setName(requestRootName);
        Document requestDoc = getRequestXML(requestName, log, dataElement);
        MRXMLUtils.getElementXpath(requestDoc, "//"+MRConst.CLIENT_DATA).setText(id);
    	// System.out.println("SessionServiceUtils:QueryToSessionServiceRequestDoc doc=" + MRXMLUtils.documentToString(requestDoc));
        return requestDoc;
    }

    public static String getServiceName(String requestName, MRLog log) throws Exception {
        if(sessionRequesttoServiceMappingDoc == null) {
        	sessionRequesttoServiceMappingDoc = MRXMLUtils.loadXMLResource(SessionServiceUtils.SESSION_REQUEST_TO_SERVICE_NAME_MAPPING, log);
    	}
        
        String serviceName = MRXMLUtils.getAttributeValueXpath(sessionRequesttoServiceMappingDoc,"//Request[@requestName='"+requestName+"']","serviceName", "");
       
        if(serviceName.isEmpty()) {
        	serviceName = SessionServiceConst.SESSIONSERVICE;
        }
        return serviceName;
    }
    public static Document getRequestXML(String requestName, MRLog log, Element dataElement) throws Exception {
    	if(requestDocTemplate == null)
    		requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
        Document requestDoc = (Document)requestDocTemplate.clone();
        MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(),MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
       // MRXMLUtils.getElementXpath(requestDoc, "//requestname").setText(requestName);

        String serviceName = getServiceName(requestName, log);
        //MRXMLUtils.getElementXpath(requestDoc, "//servicename").setText(serviceName);
        MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(),MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, serviceName);

        Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
        MRXMLUtils.addElement(DataEL, requestName + "Data").add(dataElement.detach());
        return requestDoc;
    }
    
    private static Element getDeprecatedDataElement(MRRequest request) throws Exception {
        if (request.getState() == MRRequest.STATE_EXCEPTION) {
            return  (Element) MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + request.getRequestName() + "Data").elements()
                                              .iterator().next();
        } else {
            Document result = request.getResult();
            return (Element) (result.getRootElement().elements().iterator().next());
        }
    }

    private static Element getDataElement(MRRequest request) throws Exception {
    	// System.out.println("SessionServiceUtils:getDataElement doc=" + MRXMLUtils.documentToString(request.getMessage().getDocument()));
        Element requestElement =  (Element) MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + request.getRequestName() + "Data").elements()
        .iterator().next();
        if("true".equals(requestElement.attributeValue("deprecated")))
        	return SessionServiceUtils.getDeprecatedDataElement(request);

        Element dataElement = null;
    	String requestRootName = requestElement.getName();

        if (request.getState() == MRRequest.STATE_EXCEPTION) {
            dataElement = requestElement;
            dataElement.setName(requestRootName + ServiceClient.REQUEST);
        } else {
            Document result = request.getResult();
            dataElement = (Element) (result.getRootElement().elements().iterator().next());
        	if(!requestRootName.equals(dataElement.getName())) {// Some responses do not include a root element
        		Document responseDoc = MRXMLUtils.stringToDocument("<" + requestRootName + "/>");
    			responseDoc.getRootElement().add(dataElement.detach());
    			dataElement = responseDoc.getRootElement();
        	}
            dataElement.setName(requestRootName + ServiceClient.RESPONSE);
        }
        
        return dataElement;
    }
    
    public static String responseToQueryXML(MRRequest request) throws Exception {
    	Element dataElement = SessionServiceUtils.getDataElement(request);
        return "<query xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS +  "' "+SessionServiceConst.SERVICE_VER+"='"+ MRXMLUtils.getServiceVersion(null)+"'>" + MRXMLUtils.elementToString(dataElement) + "</query>";
    }

    public static String exceptionToXML(MRException ex) throws Exception {
        int code = ex.getCode();
        String description = ex.getDescription();
        return "<" + ServiceClient.IPVS_SESSION_ERROR + " code='" + code + "'" + ">" + "<description>" + description + "</description>" + 
        		"</" + ServiceClient.IPVS_SESSION_ERROR + ">";
    }

    public static String exceptionToIQErrorXML(MRException ex) throws Exception {
        String sessionErrorXML = "<x xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" + exceptionToXML(ex) + "</x>";
        return sessionErrorXML;
    }

    public static String IQToServiceResponseXml(Element iq) throws Exception {
        String id = MRXMLUtils.getAttributeValue(iq, "id");
        String type = MRXMLUtils.getAttributeValue(iq, "type");
        String to = MRXMLUtils.getAttributeValue(iq, "to");
        int state = MRRequest.STATE_DONE;
        Element query = MRXMLUtils.getDescendentWithoutNamespace(iq, "query");
        String queryXML = MRXMLUtils.elementToString(query);
        int index = id.indexOf(".");

        if (index == -1) { // Not a session response

            return queryXML;
        }

        String requestName = id.substring(0, index);
        String clientData = id.substring(index + 1);
        String responseDataName = MRRequest.getResponseName(requestName) + "Data";
        int beginIndex = queryXML.indexOf("<", 1);
        int endIndex = queryXML.lastIndexOf("</query>");
        
        Element dataElement = MRXMLUtils.stringToElement(queryXML.substring(beginIndex, endIndex));
        // TBD: Remove Temp handling of deprecated
        // System.out.println("SessionServiceUtils:IQToServiceResponseXml refceived=" + MRXMLUtils.elementToString(dataElement));
        dataElement.setName(requestName);
        String dataXML = "<" + responseDataName + ">" + MRXMLUtils.elementToString(dataElement) + "</" + responseDataName + ">";

        if ("error".equals(type)) {
            //XPath xpath  = XPath.newInstance("//ipvs:" + ServiceClient.IPVS_SESSION_ERROR);
            //xpath.addNamespace("ipvs", IPVSMessageExtension.IPVS_SESSION_XMLNS);
            XPath xpath = MRXMLUtils.createXPath("//ipvs:" + ServiceClient.IPVS_SESSION_ERROR, "ipvs", IPVSMessageExtension.IPVS_SESSION_XMLNS);
            Element ipvserror = (Element) xpath.selectSingleNode(iq);

            // Element ipvserror = MRXMLUtils.getDescendentWithoutNamespace(iq, ServiceClient.IPVS_SESSION_ERROR);
            //xpath  = XPath.newInstance("//ipvs:description");
            //xpath.addNamespace("ipvs", IPVSMessageExtension.IPVS_SESSION_XMLNS);
            xpath = MRXMLUtils.createXPath("//ipvs:description", "ipvs", IPVSMessageExtension.IPVS_SESSION_XMLNS);

            Element descriptionElement = (Element) xpath.selectSingleNode(iq);
            Integer code = Utils.intValue(MRXMLUtils.getAttributeValue(ipvserror, "code"), 0);
            String description = MRXMLUtils.getValue(descriptionElement);
            MRException mre = new MRException(code, description);
            dataXML = "<Exception>" + mre.toString() + "</Exception>";
            state = MRRequest.STATE_EXCEPTION;
        }

        String xml = "<Response><Header userJID='" + to + "' requestNID='iq' requestName='"+requestName+"' state='"+ String.valueOf(state) +"'>" + "<ClientData>" + clientData +        		
            "</ClientData>" + "</Header><Data>" +
            dataXML + "</Data></Response>";

        return xml;
    }

    public static Document createBookmarkResponse(Document response, MRLog log, Document bookmarkResponseDoc) throws Exception {
        String bookmarkID = MRXMLUtils.getAttributeValueXpath(response, ".//Bookmark", "NID", null);
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(response, ".//MediaStreamFileResourceNID", "");
        String mediaTime = MRXMLUtils.getValueXpath(response, ".//AtTC", "");
        String mediaOffset = MRXMLUtils.getValueXpath(response, ".//Offset", "");
        String dateCreated = MRXMLUtils.getValueXpath(response, ".//DateCreated", "");
        String Title = MRXMLUtils.getValueXpath(response, ".//Title", "");
        String Comment = MRXMLUtils.getValueXpath(response, ".//Comment", "");
        String owner = MRXMLUtils.getValueXpath(response, ".//AuthorUserID", "");

        Document bookmarkResponse = MRXMLUtils.loadXMLResource(SessionServiceUtils.BOOKMARK_RESPONSE_XML, log);
        MRXMLUtils.setAttributeValueXPath(bookmarkResponse, ".//Bookmark", "id", bookmarkID);
        MRXMLUtils.setValueXpath(bookmarkResponse, ".//Owner", owner);
        MRXMLUtils.setValueXpath(bookmarkResponse, ".//DateCreated", dateCreated);
        MRXMLUtils.setAttributeValueXPath(bookmarkResponse, ".//MediaInfo", "id", mediaStreamFileNID);
        MRXMLUtils.setAttributeValueXPath(bookmarkResponse, ".//MediaInfo", "TC", mediaTime);
        MRXMLUtils.setAttributeValueXPath(bookmarkResponse, ".//MediaInfo", "offset", mediaOffset);
        MRXMLUtils.setValueXpath(bookmarkResponse, ".//Title", Title);
        MRXMLUtils.setValueXpath(bookmarkResponse, ".//Comments", Comment);
        bookmarkResponseDoc.getRootElement().add((Element) bookmarkResponse.getRootElement().clone());

        return bookmarkResponseDoc;
    }

    public static Document handleGetBookMarkList(MRRequest request, String fileId, String xpath, MRLog log, XMPPI xmpp, MRDB db,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        Document requestDoc = MRXMLUtils.loadXMLResource(SessionServiceUtils.GETBOOKMARK_LIST_XML, log);
        MRXMLUtils.getElementXpath(requestDoc, "//MediaStreamFileResourceNID").setText(fileId);
        MRXMLUtils.getElementXpath(requestDoc, ".//WhereXPath").setText(xpath);

        String serviceName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");
        String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
        MRMessage message = new MRMessage(requestDoc);
        String resourceGroup = "";
        String handlerName = "com.ipvs.mediaroomservice.handler." + requestName + "Handler";
        MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
        request = MRRequest.createRequest(request.getUserJID(), MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);

        Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        return response;
    }

    public static Document getMediaRoomDoc(String sessionID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        String whereXpath = "//MediaRoom[@id='" + sessionID + "']";
        Document mediaRoomDoc = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null,
                whereXpath, false);
        Element response = MRXMLUtils.getElementXpath(mediaRoomDoc, "//ResponseXML");

        if (response.elements().size() == 0) {
            throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, MRXMLUtils.generateErrorXML("sessionID", sessionID));
        }

        return mediaRoomDoc;
    }

    public static synchronized ArrayList<String> getFileList(Document result) throws Exception {
        ArrayList<String> fileList = new ArrayList<String>();
        List<?> playBackList = null;
        String recordXPath = ".//RecordTrack/mediadestlist/MediaDest/data/destNID";
        String playbackXPath = ".//StreamTrack/mediasourcelist/MediaSource/data";
        List<?> li = MRXMLUtils.getListXpath(result, recordXPath);

        for (Iterator<?> i = li.iterator(); i.hasNext();) {
            Element actionEL = (Element) i.next();
            String fileId = MRXMLUtils.getValue(actionEL);

            if (!"".equals(fileId) && (fileId != null)) {
                fileList.add(fileId);
            }
        }

        playBackList = MRXMLUtils.getListXpath(result, playbackXPath);

        for (Iterator<?> i = playBackList.iterator(); i.hasNext();) {
            Element actionEL = (Element) i.next();
            String pvrEnabled = MRXMLUtils.getValueXpath(actionEL, ".//pvrEnabled", "");
            String fileId = MRXMLUtils.getValueXpath(actionEL, ".//lookbackSourceNID", "");

            if (!"".equals(fileId) && !pvrEnabled.equals("true")) {
                fileList.add(fileId);
            }
        }

        return fileList;
    }
    
    public static Element createTimeLine(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, String sessionId, 
    		String trackNID, Document trackDataDoc, String requestedAction) throws Exception {
    	return createTimeLine(xmpp, request, log, db, sessionId, trackNID, trackDataDoc, requestedAction,"");
    }

    public static Element createTimeLine(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, String sessionId, 
    		String trackNID, Document trackDataDoc, String requestedAction, String timeScale) throws Exception {

    	// String logStr = "";
        String connId = "";
        String id = trackNID;
        String connGroupId = "";
        String parentTimelineId = "";

        MediaTimeline  timeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);

        Document timeLineDoc = MRXMLUtils.loadXMLResource(SessionServiceUtils.TIMELINE_INSTANCE_XML, log);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Timeline", "sessionId", sessionId);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Timeline", "connGroupId", connGroupId);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Timeline", "connId", connId);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Timeline", "id", id);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Start", "WC", String.valueOf(timeline.getStartWallclock()));
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Start", "Offset", String.valueOf(timeline.getStartOffset()));
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Start", "parentTimelineId", parentTimelineId);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Last", "WC", String.valueOf(timeline.getLastActionWallClock()));
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Last", "Offset", String.valueOf(timeline.getLastOffset()));
        if(timeScale.isEmpty()) {
        	timeScale = String.valueOf(timeline.getLastTimescale());
        }
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Last", "TimeScale", timeScale);
        MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Last", "requestedAction", timeline.getLastRequestedAction());
        // MRXMLUtils.setAttributeValueXPath(timeLineDoc, "//Last", "lastState", timeline.getLastState());

        return (Element) timeLineDoc.getRootElement().detach();
    }
    
    public static Element createConnectionTimeline(Element stream, MRLog log) throws Exception {
		Element mediaTimeline = (Element)(MRXMLUtils.getElementXpath(stream, ".//MediaTimeline")).clone();
		mediaTimeline.setName("Timeline");
		mediaTimeline.addAttribute("id", stream.attributeValue("NID"));
		Element start = mediaTimeline.element("Start");
		if(start != null)
		    start.addAttribute("parentTimelineId", "");		
		Element last = mediaTimeline.element("Last");
		if(last != null){
		String ssTC = last.attributeValue("ssTC");
		String ssWC = last.attributeValue("ssWC");
		if((ssWC != null) && (!ssWC.isEmpty())) { // Update the WC TC to be that returned by streaming server
			last.addAttribute("WC", ssWC);
			last.addAttribute("TC", ssTC);
		}
		last.addAttribute("action", null);
		last.addAttribute("lastState", null);
		last.addAttribute("sourceAction", null);
		last.addAttribute("ssTC", null);
		last.addAttribute("ssWC", null);
		}
		return mediaTimeline;

    }

    public static void updateSetupError(MRLog log, Element element, Exception e) {
        // int state = MediaRoomServiceConst.SETUP_ERROR;
        int code = MRExceptionCodes.SYSTEMERROR;
        String exceptionDescription = null;

        if (e instanceof MRException) {
            MRException mre = (MRException) e;
            code = mre.getCode();
            exceptionDescription = mre.getDescription();
        } else {
            exceptionDescription = MRException.getStackTrace(e);
        }

        MRXMLUtils.addAttribute(element, SessionServiceConst.STATE, MediaRoomServiceUtils.getStateStr(log, MediaRoomServiceConst.SETUP_ERROR));
        MRXMLUtils.addAttribute(element, SessionServiceConst.ERROR_CODE, String.valueOf(code));
        MRXMLUtils.addAttribute(element, SessionServiceConst.ERROR_DATA, exceptionDescription);
    }   
    
    public static void sendSessionMessageNotification(XMPPI xmpp,String userJID,Document messageDoc) throws MRException,Exception {
      	xmpp.sendMessage(userJID, getSessionMessageNotification(messageDoc),IPVSMessageExtension.IPVS_SESSION_XMLNS);
    }
    
    public static String getSessionMessageNotification(Document messageDoc) throws MRException,Exception {
    	return  "<x xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" +	MRXMLUtils.elementToString(messageDoc.getRootElement()) + "</x>";
    }
    
    public static String getSessionMessageNotification(Element messageEl) throws MRException,Exception {
    	return  "<x xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" +	MRXMLUtils.elementToString(messageEl) + "</x>";
    }
    

    public static void sendSessionMessageToList(XMPPI xmpp, ArrayList<String> userJIDList, Element sessionStateEl, SessionNotifier sessionNotifier) throws MRException,Exception {
    	String  sessionMessage = "<x xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" +
		MRXMLUtils.elementToString(sessionStateEl) + "</x>";
    	String sessionId = sessionStateEl.attributeValue("sessionId");
    	String notificationId = sessionId;
    	for(String userJID:userJIDList)  {
    		// xmpp.sendMessage(userJID, sessionMessage, IPVSMessageExtension.IPVS_SESSION_XMLNS);
    		String notificationData = userJID;
    		sessionNotifier.expediteNotification(BaseNotifier.ROOM_NOTIFICATION_TYPE, notificationData, notificationId, sessionMessage, sessionId); 	
    	}
    }

    public static List<Element> getMonitorAgentList(Element sourceElement, String objectType, MRLog log) throws Exception{
    	Document observerDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.OBSERVERXML, log);
		Element observerInfoEL = (Element)MRXMLUtils.getElementXpath(observerDoc,"//"+ "ObserverInfo").detach();
		String JID = MRXMLUtils.getValueXpath(sourceElement, ".//monitorAgentJID", "");
		if ((JID.isEmpty()) || (JID == null))
			return null;
		MRXMLUtils.setAttributeValue(observerInfoEL, "JID", JID);
		MRXMLUtils.setAttributeValue(observerInfoEL, "SelectFormat", "");
		MRXMLUtils.setAttributeValue(observerInfoEL, "requestHandlerClassName", "");
		MRXMLUtils.setAttributeValue(observerInfoEL, "objectType", objectType);
		List<Element> observerList = new ArrayList<Element>();
		observerList.add(observerInfoEL);
		return observerList;
    }
}
