package com.ipvs.systemservice.impl;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.ProtocolException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Random;
import java.util.StringTokenizer;

import javax.net.ssl.HttpsURLConnection;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.systemservice.filetransfer.FileTransferTable;
import com.ipvs.utils.SecurityUtils;

public class FileTransferAgent implements ServiceAgentI {
    private XMPPF xf;
    private MRLog log;
    private String userJID;
    private Thread httpUpload = null;
    private Thread httpDownload = null;
    public static final String FILEXFERSETUPREQUEST = "FileXferSetupRequest";
    public static final String FILEXFERSTARTREQUEST = "FileXferStartRequest";
    public static final String FILEXFERHTTPUPLOADREQUEST = "FileXferHttpUploadRequest";
    public static final String FILEXFERSTOPREQUEST = "FileXferStopRequest";
    public static final String STARTFILEDOWNLOADREQUEST ="StartDownloadingRequest";
    public static final String FILEXFEREVENT = "FileXferEvent";

    public static final String FILEXFERSETUP = "Init";
    public static final String FILEXFERSTARTED = "Started";
    public static final String FILEXFERSTOPPED = "Stopped";
    public static final String FILEXFERNEGOTIATING = "Negotiating";
    public static final String FILEXFERNEGOTIATED = "Negotiated";
    public static final String FILEXFERPROGRESS = "Progress";
    public static final String FILEXFERCOMPLETE = "Completed";
    public static final String FILEXFERERROR = "Error";
    private FileTransferTable ft;
    private static final int BUFFER_SIZE = 6 * 1024 * 1024;

    // Rfc1867 protocol
    private static final String field_sep = ":";
    private static final String CR_LF = "\r\n";
    private static final String TWO_DASHES = "--";
    private static String BOUNDARY;

    /**
     * The pool of ASCII chars to be used for generating a multipart boundary.
     */
    private final static char[] MULTIPART_CHARS = "-_1234567890abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ".toCharArray();

    public String getBoundry() {
	StringBuilder buffer = new StringBuilder();
	Random rand = new Random();
	int count = rand.nextInt(11) + 30; // a Random size from 30 to 40
	for (int i = 0; i < count; i++) {
	    buffer.append(MULTIPART_CHARS[rand.nextInt(MULTIPART_CHARS.length)]);
	}
	return (buffer.toString());

    }
    @Override
    public boolean handleEvent(MRClient client, Document doc, Element event) throws Exception {
	// TODO Auto-generated method stub
	return false;
    }

    @Override
    public String handleMessage(MRClient client, String fromJID, String message) throws Exception {
	String messageType = "";
	String result = null;
	com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleMessage");
	// Ignore if the message is not meant for me
	Document doc = MRXMLUtils.stringToDocument(message);
	messageType = MRXMLUtils.getAttributeValueXpath(doc, "//AgentMessage","type", "");
	if (messageType.equals(ServiceClient.REQUEST)) {
	    handleRequestMessage(client, fromJID, doc);
	} else if (messageType.equals(ServiceClient.EVENT)) {
	    handleEventMessage(client, fromJID, doc);
	} else {
	    result = handleResponseMessage(client, fromJID, doc);
	}
	result = message;
	com.ipvs.common.XMPPTransaction.endBlock("FileTransferAgent.handleMessage");

	return result;
    }

    public String handleResponseMessage(MRClient client, String fromJID, Document doc) throws Exception {
	return null;
    }

    public String handleEventMessage(MRClient client, String fromJID, Document doc) throws Exception {
	String fileXferID = "";
	String eventCode = "";
	String progress = "";
	fileXferID = MRXMLUtils.getValueXpath(doc, "//FileXferID", "");
	eventCode = MRXMLUtils.getValueXpath(doc, "//eventCode", "");
	progress = MRXMLUtils.getValueXpath(doc, "//percentageComplete", "");
	if (ft.updateFileTransferInstance(doc)) {
	    log.addInfo("FileTransferAgent: XEvent " + fromJID + ":" + fileXferID + "->" + eventCode + " " + progress);
	} else {
	    log.addInfo("FileTransferAgent: nothing in table : unknown :  XEvent " + fromJID + ":" + fileXferID + "->" + eventCode + " " + progress);
	}
	if (FileTransferAgent.FILEXFERCOMPLETE.equals(eventCode) || FileTransferAgent.FILEXFERERROR.equals(eventCode)) {
	    Element resourceInstance = this.ft.getFileTransferInstance(fileXferID);
	    String type = MRXMLUtils.getValueXpath(resourceInstance, ".//type", "");
	    if (FileTransferTable.FILEXFERTYPE_INPUT.equals(type)) {
		Element request = MRXMLUtils.getElementXpath(resourceInstance, ".//request");
		Document requestDoc = MRXMLUtils.elementToDocument(request);
		String requestorAgentJID = MRXMLUtils.getElementXpath(request, ".//message").attributeValue("from");
		int state = MRRequest.STATE_DONE;
		if (FileTransferAgent.FILEXFERERROR.equals(eventCode)) {
		    state = MRRequest.STATE_EXCEPTION;
		}
		String response = MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(request, ".//Data"));
		ServiceAgentUtils.sendAgentResponse(xf.xmpp(userJID), log, client, requestDoc, requestorAgentJID, response, this.userJID, this.getClass().getName(), state);
	    }
	}
	return null;
    }

    public String handleRequestMessage(MRClient client, String fromJID, Document doc) throws Exception {
	com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleMessage");
	String requestName =  MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");

	if (requestName.equals(FileTransferAgent.FILEXFERSETUPREQUEST)) {
	    return handleSetupRequest(client, fromJID, doc);
	} else if (requestName.equals(FileTransferAgent.FILEXFERSTARTREQUEST)) {
	    return handleStartRequest(client, fromJID, doc);
	} else if (requestName.equals(FileTransferAgent.FILEXFERSTOPREQUEST)) {
	    return handleStopRequest(client, fromJID, doc);
	} else if (requestName.equals(FileTransferAgent.FILEXFERHTTPUPLOADREQUEST)) {
	    log.addInfo("FileTRansferAgent.handleRequestMessage: "+FileTransferAgent.FILEXFERHTTPUPLOADREQUEST+" recvd");

	    log.addInfo("FileTRansferAgent.handleRequestMessage: "+FileTransferAgent.FILEXFERHTTPUPLOADREQUEST+" thread was not alive");
	    httpUpload= new Thread(new HttpFileUpload(fromJID, doc));
	    httpUpload.start();

	    // new Thread(new HttpFileUpload(fromJID, doc)).start();
	    return null;
	} else if (requestName.equals(FileTransferAgent.STARTFILEDOWNLOADREQUEST)) {		
		log.addInfo("FileTRansferAgent.handleRequestMessage: "+FileTransferAgent.STARTFILEDOWNLOADREQUEST);
		httpDownload = new Thread(new HttpFileDownload(fromJID, doc));
		httpDownload.start();
	    return null;
	} else {
	    log.addInfo("FileTransferAgent: handleRequestMessage Unknown requestName ?" + requestName);
	    com.ipvs.common.XMPPTransaction.endBlock("FileTransferAgent.handleMessage");
	    return null;
	}
    }

    @Override
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
	    String userJID, Document setServiceDomainResponseDoc) {
	this.xf = xf;
	this.log = log;
	this.userJID = userJID;
	ft = new FileTransferTable(userJID, log);

	try {        	
	    Element el = MRXMLUtils.newElement("DATA");
	    ft.getStatus(el);
	    System.out.println(MRXMLUtils.elementToString(el));
	    ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), FileTransferAgent.class.getName(), MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);
	} catch (Exception e) {
	    e.printStackTrace();
	}
    } 

    public String handleSetupRequest(MRClient client, String fromJID, Document doc) throws Exception {
		String fromFileName, toFileName, clientJID, serverJID, url;

		String requestName = MRXMLUtils.getAttributeValueXpath(doc,
				MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");		
		String fileXferID = MRXMLUtils.getValueXpath(doc, "//FileXferID", "");
		log.addInfo("FileTransferAgent:FileXferID" + fileXferID);
		serverJID = MRXMLUtils.getValueXpath(doc, "//fromAgentJID", "");
		clientJID = MRXMLUtils.getValueXpath(doc, "//toAgentJID", "");
		fromFileName = MRXMLUtils.getValueXpath(doc, "//fromFileName", "");
		toFileName = MRXMLUtils.getValueXpath(doc, "//toFileName", "");
		url = MRXMLUtils.getValueXpath(doc, "//url", "");
		
		if (url != null && !url.isEmpty())
		{
			log.addInfo("FileTransferAgent: User JID: " + userJID
					+ ",Request Name:" + requestName + ", ServerJID: " + serverJID
					+ ", From File: " + fromFileName + "->" + clientJID
					+ ", To File: " + toFileName + ", URL=" + url);
	
			ft.addFileTransferInstance(FileTransferTable.FILEXFERTYPE_INPUT, doc);
	
			XMPPI xmpp = xf.xmpp(userJID);
			Document fileTransferEventDoc = MRXMLUtils.loadXMLResource(
					SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, log);
			MRXMLUtils.setValueXpath(fileTransferEventDoc, "//FileXferID",
					fileXferID);
			MRXMLUtils.setValueXpath(fileTransferEventDoc, "//eventCode",
					FILEXFERSTARTED);
			ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, fromJID,
					MRConst.FTP_SERVICE, FILEXFEREVENT,
					fileTransferEventDoc.getRootElement(),
					"FileTransferAgent: Sending FileTransferRequest from "
							+ SystemServiceConst.FILETRANSFERAGENT + " to "
							+ SystemServiceConst.FILETRANSFERAGENT);
	
			// Send a start downloading db backup created on the server
	
			HttpURLConnection connection = getConnection(url);
			InputStream anURLInputStream = connection.getInputStream();
			BufferedOutputStream writeToDBBackupFile = new BufferedOutputStream(
					new FileOutputStream(new File(toFileName)));
			final byte[] buffer = new byte[1024];
			int read = 0;
	
			while (-1 != (read = anURLInputStream.read(buffer))) {
				writeToDBBackupFile.write(buffer, 0, read);
			}
	
			writeToDBBackupFile.flush();
			writeToDBBackupFile.close();
			anURLInputStream.close();
	
			final int responseCode = connection.getResponseCode();
			if (responseCode != HttpsURLConnection.HTTP_OK) {
				final String responseMessage = connection.getResponseMessage();
				log.addInfo("Error while getting file from URL='" + url
						+ "' ResponseCode=" + responseCode + " ResponseMsg="
						+ responseMessage);
			} else {
				MRXMLUtils.setValueXpath(fileTransferEventDoc, "//eventCode",
						FILEXFERCOMPLETE);
				MRXMLUtils.setValueXpath(fileTransferEventDoc,
						"//percentageComplete", "100");
				
				ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, clientJID,
						MRConst.FTP_SERVICE, FILEXFEREVENT,
						fileTransferEventDoc.getRootElement(),
						"FileTransferAgent: Sending FileTransferRequest Event from "
								+ SystemServiceConst.FILETRANSFERAGENT + " to "
								+ SystemServiceConst.FILETRANSFERAGENT);
	
				MRXMLUtils.setValueXpath(fileTransferEventDoc, "//eventCode",
						FILEXFERSTOPPED);
				MRXMLUtils.setValueXpath(fileTransferEventDoc,
						"//percentageComplete", "100");
				
				ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, clientJID,
						MRConst.FTP_SERVICE, FILEXFEREVENT,
						fileTransferEventDoc.getRootElement(),
						"FileTransferAgent: Sending FileTransferRequest Event from "
								+ SystemServiceConst.FILETRANSFERAGENT + " to "
								+ SystemServiceConst.FILETRANSFERAGENT);
			}
		}
		/*
		 * This block is introduced to maintain legacy check and will be 
		 * removed after all the flows moved to http instead of xmpp
		 */
		else
		{
			com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleMessage");
			String responseName = requestName.replace(ServiceClient.REQUEST, ServiceClient.RESPONSE);
			Element fileTransferResponse;
			int responseState = MRRequest.STATE_DONE; // Should I be using something else : Shanker
			StringTokenizer st;
			String leafNode = "";
			String responseFileName = SystemServiceConst.FILEXFERSETUPRESPONSEFILE;
			st = new StringTokenizer(fromFileName, "/\\");
			while (st.hasMoreTokens()) {
			    leafNode = st.nextToken();
			}
			log.addInfo("FileTransferAgent:" + requestName + " " + serverJID + "," + fromFileName + "->" + clientJID + "," + toFileName + "*" + leafNode);
			// Send a start file transfer message to the source
			Document startMessageDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFERSTARTREQUESTFILE, log);
			MRXMLUtils.setValueXpath(startMessageDoc, "//FileXferID", fileXferID);
			MRXMLUtils.setValueXpath(startMessageDoc, "//fromFileName", fromFileName);
			Element startMessage = MRXMLUtils.getElementXpath(startMessageDoc, "//Request");
			String data = MRXMLUtils.elementToString(startMessage);
			ft.addFileTransferInstance(FileTransferTable.FILEXFERTYPE_INPUT, doc);

			xf.xmpp(userJID).getFile(serverJID, leafNode, toFileName, fileXferID);
			// provision the smack listener to accept an incoming file transfer

			XMPPI xmpp = xf.xmpp(userJID);
			ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null,SystemServiceConst.FILETRANSFERAGENT, serverJID, SystemServiceConst.FILETRANSFERAGENT, FileTransferAgent.FILEXFERSTARTREQUEST, data, "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT, null,this.userJID,MRConst.FTP_SERVICE, fileXferID);

			Document FileTransferEventDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, log);
			MRXMLUtils.setValueXpath(FileTransferEventDoc, "//FileXferID", fileXferID);
			MRXMLUtils.setValueXpath(FileTransferEventDoc, "//eventCode", FILEXFERSTARTED);
			ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, fromJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
			ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, clientJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
			ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, serverJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);

			String logdata = "";
			Document responseDoc = MRXMLUtils.loadXML(responseFileName, log);
			MRXMLUtils.setValueXpath(responseDoc, "//FileXferID", fileXferID);
			fileTransferResponse = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");

			ServiceAgentUtils.sendAgentResponseMessage(xmpp, log, null, fromJID, SystemServiceConst.FILETRANSFERAGENT, requestName, MRXMLUtils.elementToString(fileTransferResponse), "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT, responseState, logdata, doc);
			com.ipvs.common.XMPPTransaction.endBlock("FileTransferAgent.handleMessage");
		}

		return null;
	}

	public static HttpsURLConnection getConnection(final String anUrl)
			throws Exception, MalformedURLException {
		URL url = new URL(anUrl);
		HttpsURLConnection connection = (HttpsURLConnection)url 
				.openConnection();
		connection.setSSLSocketFactory(SecurityUtils.getTrustServerConnectionFactory());
		connection.setHostnameVerifier(SecurityUtils.getHostVerifier());
		connection.setDoOutput(true);
		connection.setDoInput(true);
		connection.setAllowUserInteraction(false); // If we get here then it
													// looks to be ok and we can
													// stream the contents.
		connection.setUseCaches(false);
		connection.setDefaultUseCaches(false);

		// set the content type to indicate that we're operating on binary data
		connection.setRequestProperty("Content-Type",
				"application/octet-stream");

		return connection;
	}

    public String handleStartRequest(MRClient client, String fromJID, Document doc) throws Exception {
	String requestName = "";
	String responseName = "";
	String responseFileName = "";
	String fileXferID = "";
	Element fileTransferResponse;
	int responseState = MRRequest.STATE_DONE; // Should I be using something else : Shanker

	com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleMessage");
	requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
	responseName = requestName.replace(ServiceClient.REQUEST, ServiceClient.RESPONSE);

	String fromFileName;

	responseFileName = SystemServiceConst.FILEXFERSTARTRESPONSEFILE;
	fromFileName = MRXMLUtils.getValueXpath(doc, "//fromFileName", "");
	fileXferID = MRXMLUtils.getValueXpath(doc, "//FileXferID", "");
	log.addInfo("FileTransferAgent:" + requestName + " " + fromJID + "," + fromFileName + ":" + fileXferID);

	ft.addFileTransferInstance(FileTransferTable.FILEXFERTYPE_OUTPUT, doc, fromJID);

	//validate the file just before sending....
	boolean send_failed = false;
	String msg_info = "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT;
	File file = new File(fromFileName);
	if (file.exists() && file.canRead() && file.length() > 0) {
	    try {
		xf.xmpp(userJID).sendFile(fromJID, fromFileName, fileXferID);
	    } catch (Exception ex) {
		ex.printStackTrace();
		send_failed = true;
		responseState = MRRequest.STATE_EXCEPTION;
		msg_info = "FileTransferAgent: Erorr Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT + ", Error=" + ex.getMessage();
	    }
	} else {
	    send_failed = true;
	    responseState = MRRequest.STATE_EXCEPTION;
	    msg_info = "FileTransferAgent: Erorr Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT + ", Error=file '" + fromFileName + "' does no exist or empty ";
	}
	// initiate an outgoing file transfer                       

	String logdata = "";
	Document responseDoc = MRXMLUtils.loadXML(responseFileName, log);
	MRXMLUtils.setValueXpath(responseDoc, "//FileXferID", fileXferID);
	fileTransferResponse = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");

	ServiceAgentUtils.sendAgentResponseMessage(xf.xmpp(userJID), log, null, fromJID, SystemServiceConst.FILETRANSFERAGENT, requestName, MRXMLUtils.elementToString(fileTransferResponse), msg_info, responseState, logdata, doc);
	if (send_failed) {
	    sendErrorEvent(client, fromJID, responseDoc, fileXferID, SystemServiceConst.FILETRANSFER_SEND_FILE_ERROR, msg_info);
	}
	com.ipvs.common.XMPPTransaction.endBlock("FileTransferAgent.handleMessage");

	return null;
    }

    public String handleStopRequest(MRClient client, String fromJID, Document doc) throws Exception {
	String requestName = "";
	String responseName = "";
	String responseFileName = "";
	String fileXferID = "";
	Element fileTransferResponse;
	String logdata = "";
	int responseState = MRRequest.STATE_DONE; // Should I be using something else : Shanker

	requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
	responseName = requestName.replace(ServiceClient.REQUEST, ServiceClient.RESPONSE);

	// Stop any transfers if incoming transfer exists then
	// send a stop to the Server
	String clientJID, serverJID;
	String fileName = "";
	Element fileXfer = null;
	String percentageComplete = null;
	String type = null;

	XMPPI xmpp = xf.xmpp(userJID);

	com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleMessage");
	responseFileName = SystemServiceConst.FILEXFERSTOPRESPONSEFILE;
	fileXferID = MRXMLUtils.getValueXpath(doc, "//FileXferID", "");
	fileXfer = ft.getFileTransferInstance(doc);
	if (fileXfer != null) {
	    log.addInfo("FileTransferAgent:" + requestName + " " + fromJID + ":" + fileXferID);
	    type = MRXMLUtils.getValueXpath(fileXfer, ".//type", "");
	    if (type.compareToIgnoreCase(FileTransferTable.FILEXFERTYPE_OUTPUT) == 0) {
		clientJID = MRXMLUtils.getValueXpath(fileXfer, ".//clientJID", "");
		percentageComplete = MRXMLUtils.getValueXpath(fileXfer, "//percentageComplete", "");
		fileName = MRXMLUtils.getValueXpath(fileXfer, ".//fromFileName", "");
		xmpp.sendFileStop(clientJID, fileName);
		Document FileTransferEventDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, log);
		MRXMLUtils.setValueXpath(FileTransferEventDoc, "//FileXferID", fileXferID);
		MRXMLUtils.setValueXpath(FileTransferEventDoc, "//eventCode", FILEXFERSTOPPED);
		MRXMLUtils.setValueXpath(FileTransferEventDoc, "//percentageComplete", percentageComplete);

		String user = xmpp.getXMPPUserJid();
		serverJID = MRXMLUtils.getValueXpath(fileXfer, ".//serverJID", "");
		ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, clientJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest Event from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
		ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, xmpp.getXMPPUserJid(), MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest Event from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
		if (!user.equals(fromJID)) {
		    ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, fromJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest Event from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
		}
		if (!serverJID.equals("") && !serverJID.equals(fromJID)) {
		    ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, serverJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest Event from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
		}
	    } else {
		serverJID = MRXMLUtils.getValueXpath(fileXfer, ".//serverJID", "");
		// Send a start file transfer message to the source
		Document stopMessageDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFERSTOPREQUESTFILE, log);
		MRXMLUtils.setValueXpath(stopMessageDoc, "//FileXferID", fileXferID);
		Element startMessage = MRXMLUtils.getElementXpath(stopMessageDoc, "//Request");
		String data = MRXMLUtils.elementToString(startMessage);

		ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null,SystemServiceConst.FILETRANSFERAGENT, serverJID, SystemServiceConst.FILETRANSFERAGENT, FileTransferAgent.FILEXFERSTOPREQUEST, data, "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT, null,this.userJID,MRConst.FTP_SERVICE, fileXferID);
		xmpp.getFileStop(fromJID, fileName, fileName);

		Document FileTransferEventDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, log);
		MRXMLUtils.setValueXpath(FileTransferEventDoc, "//FileXferID", fileXferID);
		MRXMLUtils.setValueXpath(FileTransferEventDoc, "//eventCode", FILEXFERSTOPPED);
		MRXMLUtils.setValueXpath(FileTransferEventDoc, "//percentageComplete", percentageComplete);
		serverJID = MRXMLUtils.getValueXpath(fileXfer, ".//serverJID", "");
		MRLog.println("FileTransferAgent: handleStopRequest : GETTER to ? '" + serverJID + "' '" + fromJID + "'  :" + fileXferID);
		if (!serverJID.equals("")) {
		    ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, serverJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: Sending FileTransferRequest Event from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
		}

	    }
	    ft.deleteFileTransferInstance(fileXferID);
	} else {
	    log.addInfo("FileTransferAgent:noting in table: " + requestName + " " + fromJID + ":" + fileXferID);
	    responseState = MRRequest.STATE_EXCEPTION;
	    Exception e = new MRException(MRExceptionCodes.FILE_XFER_INSTANCE_NOT_FOUND, "File Transfer Instance not Found");
	    logdata = e.toString();
	}


	Document responseDoc = MRXMLUtils.loadXML(responseFileName, log);
	MRXMLUtils.setValueXpath(responseDoc, "//FileXferID", fileXferID);
	fileTransferResponse = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");

	ServiceAgentUtils.sendAgentResponseMessage(xmpp, log, null, fromJID, SystemServiceConst.FILETRANSFERAGENT, requestName, MRXMLUtils.elementToString(fileTransferResponse), "FileTransferAgent: Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT, responseState, logdata, doc);
	com.ipvs.common.XMPPTransaction.endBlock("FileTransferAgent.handleMessage");
	return null;
    }

    public void sendErrorEvent(MRClient client, String fromJID, Document doc, String fileXferID, int errorCode, String error) throws Exception {
	Element resourceInstance = this.ft.getFileTransferInstance(fileXferID);
	if (resourceInstance == null) {
	    MRLog.println("FileTransferAgent: Not found fileXferID '" + fileXferID + "' fromJID='" + fromJID + "'. Can not send error from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
	    return;
	}
	XMPPI xmpp = xf.xmpp(userJID);

	String type = MRXMLUtils.getValueXpath(resourceInstance, ".//type", "");
	Element request = MRXMLUtils.getElementXpath(resourceInstance, ".//request");
	String requestorAgentJID = "";
	if (FileTransferTable.FILEXFERTYPE_INPUT.equals(type)) {
	    //its getFile agent, send to who created the request
	    Document requestDoc = MRXMLUtils.elementToDocument(request);
	    FileSMUtils.addErrorToResponse(requestDoc.getRootElement(), errorCode, error);
	    String response = MRXMLUtils.elementToString(request);
	    requestorAgentJID = MRXMLUtils.getElementXpath(request, ".//message").attributeValue("from");
	    ServiceAgentUtils.sendAgentResponse(xmpp, log, client, requestDoc, requestorAgentJID, response, this.userJID, this.getClass().getName(), MRRequest.STATE_EXCEPTION);
	} else {
	    //send to getFile agent
	    Document FileTransferEventDoc = MRXMLUtils.loadXMLResource(SystemServiceConst.FILEXFER_EVENT_INSTANCE_XML, log);
	    MRXMLUtils.setValueXpath(FileTransferEventDoc, "//FileXferID", fileXferID);
	    MRXMLUtils.setValueXpath(FileTransferEventDoc, "//eventCode", FILEXFERERROR);
	    requestorAgentJID = MRXMLUtils.getElementXpath(request, ".//message").attributeValue("from"); //the get hwo sent 
	    ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, requestorAgentJID, MRConst.FTP_SERVICE, FILEXFEREVENT, FileTransferEventDoc.getRootElement(), "FileTransferAgent: sendErrorEvent  " + fileXferID + " from " + SystemServiceConst.FILETRANSFERAGENT + " to " + SystemServiceConst.FILETRANSFERAGENT);
	}
    }

    @Override
    public void reset() throws Exception {
    	log.addInfo("LOGOUT Event called");
	if(httpUpload.isAlive()) {
		log.addInfo("Interrupted: httpUpload");
        httpUpload.interrupt();
        httpUpload.join();
    }
	
	if(httpDownload != null) {
        httpDownload.interrupt();
    }
    }

    /*private boolean canWrite(File file) throws Exception {
        if (file != null) {
            if (!file.exists()) {
                try {
                    file.getParentFile().mkdirs();
                    file.createNewFile();
                } catch (Exception e) {
                    //throw new Exception("Could not create file to write too", e);
                    log.addInfo("FileTransferAgent:canWrite: Error:" + "Could not create file to write to " + file.getAbsolutePath(), MRLog.INFO, MRLog.NOTIFY);
                    e.printStackTrace();
                    return false;
                }
            }
            if (!file.canWrite()) {
                //throw new Exception("Cannot write to provided file");
                log.addInfo("FileTransferAgent:canWrite: Error:" + "Cannot write to provided file " + file.getAbsolutePath(), MRLog.INFO, MRLog.NOTIFY);
                return false;
            }
        }
        return true;
    }

    private boolean canRead(File file) throws Exception {
        if (file == null || !file.exists() || !file.canRead()) {
            log.addInfo("FileTransferAgent:canRead: Error:" + "Could not read file  " + file.getAbsolutePath(), MRLog.INFO, MRLog.NOTIFY);
            return false;
        }
        return true;
    }*/


    private class HttpFileUpload implements Runnable {

	String fromJID;
	Document doc;
	String responseFileName = SystemServiceConst.FILEXFERHTTPUPLOADRESPONSEFILE;
	Element fileTransferResponse;
	int responseState;
	String responseInfo;

	private HttpFileUpload(String fromJID, Document doc) {

	    this.fromJID = fromJID;
	    this.doc = doc;
	}

	@Override
	public void run() {
	    boolean keepRunning = true;
	    HttpsURLConnection urlConnection = null;
		OutputStream out = null;
		while(!Thread.currentThread().isInterrupted() && keepRunning){
			try {			
	    	
	    		com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleHttpUpload");	

	    		responseState = MRRequest.STATE_DONE; 
	    		responseInfo = "FileTransferAgent.handleHttpFileUploadRequest: Sending FileTransferRequest from " + MediaRoomServiceConst.APPSERVERAGENT + " to " + FileTransferAgent.FILEXFERHTTPUPLOADREQUEST;
	    		com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleMessage");		  
	    		String queueID = MRXMLUtils.getAttributeValueXpath(doc.getRootElement(), "//"+FileTransferAgent.FILEXFERHTTPUPLOADREQUEST+"Data", "byNID", "");
	    		log.addInfo("FileTransfer Agent queueID is: "+queueID);
	    		String url = MRXMLUtils.getValueXpath(doc, "//url", "");
	    		String filePath = MRXMLUtils.getValueXpath(doc, "//filePath", "");
	    		
	    		if(!this.uploadFiles(filePath, url,urlConnection,out)){
	    			responseState = MRRequest.STATE_EXCEPTION;
	    			responseInfo = "FileTransferAgent.handleHttpFileUploadRequest: Server error while uploading file to "+url;
	    		}		  	
	    		com.ipvs.common.XMPPTransaction.endBlock("FileTransferAgent.handleMessage");
	    		keepRunning = false;
	    	}
	    catch(InterruptedException consumed){
	    	log.addInfo("FileTransfer Agent Http transaction is Interrupted");		
	    	responseState = MRRequest.STATE_EXCEPTION;
	    	responseInfo = "FileTransferAgent.handleHttpFileUploadRequest: Upload process was interrupted because of log out event";
	    	if(out != null){
	    		try {
	    			out.flush();
	    			out.close();
	    		} catch (IOException e) {
	    			// TODO Auto-generated catch block
	    			e.printStackTrace();
	    		}

	    	}
	    	if(urlConnection != null){
	    		urlConnection.disconnect();

	    	}
	    	Thread.currentThread().interrupt();
	    } catch (Exception ex) {
	    	log.addInfo("FileTransgerAgent.HTTPFileUpload: Exception occured: "+ex.getMessage(), MRLog.OK, MRLog.NOTIFY);
	    	ex.printStackTrace();
	    	responseState = MRRequest.STATE_EXCEPTION;
	    	responseInfo = "FileTransferAgent.handleHttpFileUploadRequest: Exception occured while transfering the file";
	    	Thread.currentThread().interrupt();
	    }finally{
	    	try{
	    		XMPPI xmpp = xf.xmpp(userJID);
	    		String requestName = MRXMLUtils.getAttributeValueXpath(doc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
	    		String queueID = MRXMLUtils.getAttributeValueXpath(doc.getRootElement(), "//"+FileTransferAgent.FILEXFERHTTPUPLOADREQUEST+"Data", "byNID", "");
	    		String responseName = requestName.replace(ServiceClient.REQUEST, ServiceClient.RESPONSE);
	    		Document responseDoc = MRXMLUtils.loadXML(responseFileName, log);
	    		MRXMLUtils.setValueXpath(responseDoc, "//QueueID", queueID);
	    		fileTransferResponse = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");
	    		String requestNID = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.REQUEST_NID, "none");
	    		String clientdata = MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_DATA, "none");
	    		ServiceAgentUtils.sendAgentResponseMessage(xmpp, log, null, fromJID, MediaRoomServiceConst.APPSERVERAGENT, MediaRoomServiceConst.APPSERVERAGENT,FileTransferAgent.FILEXFERHTTPUPLOADREQUEST,
	    				MRXMLUtils.elementToString(fileTransferResponse), responseInfo, responseState, "",requestNID, userJID, clientdata);	
	    	}catch(Exception exc){
	    		log.addInfo("FileTransgerAgent.HTTPFileUpload: Exception occured: "+exc.getMessage(), MRLog.OK, MRLog.NOTIFY);
	    		exc.printStackTrace();
	    	}
	    }
		}


	}
	public boolean uploadFiles(String filePath, String strUrl, HttpsURLConnection urlConnection, OutputStream out) throws Exception {
		List<File> list = new ArrayList<File>();

		list.add(new File(filePath));//("C:\\Users\\nehut\\Documents\\bug_45351\\default.mediastreamfileresourcelist.msfrl_23b8d464-6a87-4beb-a666-3388b712607e.mkd"));
		try {
			URL url = new URL(strUrl);//("http://10.51.48.170:3100/mediajs/file/upload");		   
			return(this.uploadFilesToNodeServer(url, list,urlConnection,out));		   
		} catch (MalformedURLException e) {		   
			e.printStackTrace();
			return false;
		}

	}

	public boolean uploadFilesToNodeServer(URL url, List<File> files,HttpsURLConnection urlConnection, OutputStream out) {
		BOUNDARY = getBoundry();
		
		try {
			urlConnection = (HttpsURLConnection) url.openConnection();
			urlConnection.setSSLSocketFactory(SecurityUtils.getTrustServerConnectionFactory());
			urlConnection.setHostnameVerifier(SecurityUtils.getHostVerifier());
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		try {
			urlConnection.setRequestMethod("POST");

			urlConnection.setDoOutput(true);
			urlConnection.setChunkedStreamingMode(BUFFER_SIZE);
			urlConnection.setRequestProperty("Content-Type", "multipart/form-Data; boundary=" + BOUNDARY + "; charset = UTF-8");
			urlConnection.setRequestProperty("Content-Length", String.valueOf(files.get(0).length()));
		} catch (ProtocolException e) {
			e.printStackTrace();
			return false;
		}
		
		try {
			out = new BufferedOutputStream(urlConnection.getOutputStream()); // request
		} catch (IOException e) {
			
			urlConnection.disconnect();
			e.printStackTrace();
			return false;
		}
		FileInputStream fStream = null;
		try {
			for (File file : files) {
				StringBuilder form = new StringBuilder();

				form.append(TWO_DASHES + BOUNDARY + CR_LF);
				form.append("Content-Disposition" + field_sep + "form-Data; name = \"name\"" + CR_LF);
				form.append("Content-Type" + field_sep + "text/Plain; charset = UTF-8" + CR_LF);
				form.append("Content-Transfer-Encoding" + field_sep + "8bit" + CR_LF);
				form.append(CR_LF);
				form.append("mkd grasping wow");
				form.append(CR_LF);

				form.append(TWO_DASHES + BOUNDARY + CR_LF);
				form.append("Content-Disposition" + field_sep + "form-Data; name=\"file\"; filename=\"" + file.getName() + "\"" + CR_LF);
				form.append("Content-Type" + field_sep + "application/zip; charset = UTF-8" + CR_LF);
				form.append("Content-Transfer-Encoding" + field_sep + "binary" + CR_LF);
				form.append(CR_LF);

				out.write(form.toString().getBytes("UTF-8"));
				fStream = new FileInputStream(file);
				byte[] buffer = new byte[BUFFER_SIZE];
				int length = -1;
				while ((length = fStream.read(buffer)) != -1) {
					out.write(buffer, 0, length);
					out.flush();
				}
				out.write(CR_LF.getBytes("UTF-8"));
				out.write((TWO_DASHES + BOUNDARY + TWO_DASHES + CR_LF).getBytes("UTF-8"));
				out.flush();

			}
			try {
				out.write((TWO_DASHES + BOUNDARY + TWO_DASHES + CR_LF).getBytes("UTF-8"));
				out.flush();
				out.close();
				fStream.close();
			} catch (UnsupportedEncodingException e) {

				e.printStackTrace();
				return false;
			} catch (IOException e) {

				e.printStackTrace();
				return false;
			}
		} catch (IOException e) {
			try {

				out.close();
				e.printStackTrace();
			} catch (IOException e1) {
				e1.printStackTrace();
				return false;
			}

			urlConnection.disconnect();
			return false;
		} finally {
			try {
				if (fStream != null) {
					fStream.close();
				}
			} catch (IOException e) {
				return false;
			}

		}

		return(getResponseJava(urlConnection));
	}

	private boolean getResponseJava(HttpURLConnection urlConnection) {
	    // read & parse the response
	    InputStream is;
	    try {
		if(urlConnection.getResponseCode() == HttpURLConnection.HTTP_OK){
		    is = urlConnection.getInputStream();
		    StringBuilder response = new StringBuilder();
		    byte[] respBuffer = new byte[4096];
		    while (is.read(respBuffer) >= 0) {
			response.append(new String(respBuffer).trim());
		    }
		    is.close();
		    log.addInfo("FileTransferAgent.UploadFiletoNodeServer: The response from server is: " + response.toString());
		    return true;
		} else {
            return false;
        }
	    } catch (IOException e) {
		e.printStackTrace();
		return false;
	    }

	}
    }

	private class HttpFileDownload implements Runnable {

		String fromJID;
		Document doc;
		
		private HttpFileDownload(String fromJID, Document doc) {
			this.fromJID = fromJID;
			this.doc = doc;
		}

		@Override
		public void run() {
			try {
				String type = MRXMLUtils.getValueXpath(doc, "//type","");
				if (type!=null && type.equalsIgnoreCase("export")) {
					exportMediaFiles(type);
				} else if (type!=null && (type.equalsIgnoreCase("reports") || type.equalsIgnoreCase("dbBackup"))) {
					downloadResource(type);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		private void downloadResource(String type) throws Exception {
			
			String responseName = "";
			String responseFileName = "";
			String state ="";
			String percentageComplete ="";
			String logdata = "";
			String toFileName, clientJID, url;
			int responseState = MRRequest.STATE_DONE;			
			String requestName = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
			responseName = requestName.replace(ServiceClient.REQUEST, ServiceClient.RESPONSE);
			responseFileName = SystemServiceConst.STARTDOWNLOADRESPONSEFILE;
			clientJID = MRXMLUtils.getValueXpath(doc, "//toAgentJID", "");
			toFileName = MRXMLUtils.getValueXpath(doc, "//folderPath", "");
			url = MRXMLUtils.getValueXpath(doc, "//url", "");
			
			String clientData= MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_DATA, "none");
			Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
			MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);

			Element header = MRXMLUtils.getElementXpath(userRequest.getMessage().getDocument(), "//Header");
			String serviceName = MRXMLUtils.getAttributeValue(header, "serviceName", "");
			String fromReponseHandler = MRXMLUtils.getAttributeValue(header, "requestName", "");		
			String msg_info = "FileTransferAgent: Sending StartDownloadResponse from " + SystemServiceConst.FILETRANSFERAGENT + " to " + fromReponseHandler;
			log.addInfo("FileTransferAgent: User JID: " + userJID + ",Request Name:" + requestName + "->" + clientJID + ", To File: " + toFileName + ", URL=" + url);

			// Send a start downloading db backup created on the server
			responseState = downloadFile(url, toFileName, msg_info, responseState, type);

			if (responseState != MRRequest.STATE_DONE) {
				log.addInfo("Error while getting file from URL='" + url + "' ResponseCode=" + responseState);
					state="Error";					
				}else{
					state="Completed";					
				}			
			Document responseDoc = MRXMLUtils.loadXML(responseFileName, log);
			MRXMLUtils.setValueXpath(responseDoc, "//state", state);
			MRXMLUtils.setValueXpath(responseDoc, "//percentageComplete", percentageComplete);
			Element fileDownloadResponse = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");
			String requestNID = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.REQUEST_NID, "none");
			String clientdata = MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_DATA, "none");
			ServiceAgentUtils.sendAgentResponseMessage(xf.xmpp(userJID), log, null, fromJID, MediaRoomServiceConst.APPSERVERAGENT, serviceName,
					fromReponseHandler, MRXMLUtils.elementToString(fileDownloadResponse), msg_info, responseState, logdata, requestNID, userJID, clientdata);
			
		}

		private void exportMediaFiles(String type) {
			try {

				com.ipvs.common.XMPPTransaction.startBlock("FileTransferAgent.handleHttpDownload");

				String requestName = "";
				String responseName = "";
				String responseFileName = "";
				String fileNID = "";
				Element fileDownloadResponse;

				Element downloadEl = MRXMLUtils.getElementXpath(doc, "//StartDownloading");
				String fileURL = MRXMLUtils.getAttributeValue(downloadEl, "url");
				String msg_info = "FileTransferAgent: Sending StartDownloadResponse from " + SystemServiceConst.FILETRANSFERAGENT + " to " + MRConst.PUBLISH_FILE_DOWNLOAD_REQUEST;

				String folderPath = MRXMLUtils.getAttributeValue(downloadEl, "folderPath");
				String format = MRXMLUtils.getAttributeValue(downloadEl, "format");
				String streamType = MRXMLUtils.getAttributeValue(downloadEl, "streamType", "");
				String title = MRXMLUtils.getAttributeValue(downloadEl, "title", "");
				String new_UUID = MRXMLUtils.getAttributeValue(downloadEl, "new_UID", "");
				title = title.replaceAll("[/*:?\"<>',;']", "-").replace("\\", "-");
				String fileName = title + "_" + new_UUID;
				String saveFilePath = folderPath + File.separator + fileName;
				log.addInfo("saveFilePath ::::" + saveFilePath);
				fileNID = MRXMLUtils.getAttributeValue(downloadEl, "byNID");

				int responseState = MRRequest.STATE_DONE;
				requestName = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
				responseName = requestName.replace(ServiceClient.REQUEST, ServiceClient.RESPONSE);
				responseFileName = SystemServiceConst.STARTDOWNLOADRESPONSEFILE;

				if (format.equalsIgnoreCase(FileSM.MEDIA_FORMAT_BOTH)) {
					// download for Proprietary format
					responseState = downloadFile(fileURL.replace("both", "proprietary"), saveFilePath, msg_info, responseState, type);

					if (responseState != MRRequest.STATE_EXCEPTION) {
						// check for Standard format if stream Type is not V2D
						// and UDP
						if (!streamType.equals(MediaRoomServiceConst.V2D) && !streamType.equals(MediaRoomServiceConst.UDP)) {
							responseState = downloadFile(fileURL.replace("both", "standard"), saveFilePath, msg_info, responseState, type);
						}
					}
				} else if (format.equalsIgnoreCase(FileSM.MEDIA_FORMAT_STANDARD)) {
					if (streamType != null && !streamType.isEmpty() && !streamType.equals(MediaRoomServiceConst.V2D) && !streamType.equals(MediaRoomServiceConst.UDP)) {
						responseState = downloadFile(fileURL, saveFilePath, msg_info, responseState, type);
					} else {
						responseState = downloadFile(fileURL.replace("standard", "proprietary"), saveFilePath, msg_info, responseState, type);
					}

				} else if (format.equalsIgnoreCase(FileSM.MEDIA_FORMAT_PROPRIETARY)) {
					responseState = downloadFile(fileURL, saveFilePath, msg_info, responseState, type);
				} else {
					responseState = MRRequest.STATE_EXCEPTION;
				}

				String logdata = "";
				Document responseDoc = MRXMLUtils.loadXML(responseFileName, log);
				MRXMLUtils.setValueXpath(responseDoc, "//FileNID", fileNID);
				fileDownloadResponse = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");

				String requestNID = MRXMLUtils.getAttributeValueXpath(doc, MRConst.HEADER_XPATH, MRConst.REQUEST_NID, "none");
				String clientdata = MRXMLUtils.getValueXpath(doc, "//" + MRConst.CLIENT_DATA, "none");
				ServiceAgentUtils.sendAgentResponseMessage(xf.xmpp(userJID), log, null, fromJID, MediaRoomServiceConst.APPSERVERAGENT, MRConst.SESSION_RESOURCE_SERVICE,
						MRConst.PUBLISH_FILE_DOWNLOAD_REQUEST, MRXMLUtils.elementToString(fileDownloadResponse), msg_info, responseState, logdata, requestNID, userJID, clientdata);

				com.ipvs.common.XMPPTransaction.endBlock("FleTransgerAgent.handleHttpDownload");
			} catch (Exception e) {
				log.addInfo("FleTransgerAgent.HttpFileDownload: Exception occured: " + e.getMessage(), MRLog.OK, MRLog.NOTIFY);
			}
		}

		public int downloadFile(String fileURL, String saveFilePath, String msg_info, int responseState, String type) throws Exception {

			URL url = new URL(fileURL);
			HttpsURLConnection httpConn = (HttpsURLConnection) url.openConnection();
			httpConn.setSSLSocketFactory(SecurityUtils.getTrustServerConnectionFactory());
			httpConn.setHostnameVerifier(SecurityUtils.getHostVerifier());
			int responseCode = httpConn.getResponseCode();
			log.addInfo("responseCode ::::" + responseCode);
			if (responseCode == HttpsURLConnection.HTTP_OK) {
				log.addInfo("responseCoden OK ::::" + responseCode);

				if (type.equalsIgnoreCase("export")) {
					String fileExtention = "";
					String disposition = httpConn.getHeaderField("Content-Disposition");

					if (disposition != null) {
						// extracts file name from header field
						int index = disposition.indexOf("filename=");
						if (index > 0) {
							String name = disposition.substring(index + 9);
							fileExtention = name.substring((name.indexOf(".") - 1), name.length());
							saveFilePath += fileExtention;
							log.addInfo("saveFilePath with extention::" + saveFilePath);
						}
					}
				}
				InputStream inputStream = httpConn.getInputStream();
				FileOutputStream outputStream = new FileOutputStream(saveFilePath);
				int bytesRead = -1;
				byte[] buffer = new byte[BUFFER_SIZE];
				while ((bytesRead = inputStream.read(buffer)) != -1) {
					outputStream.write(buffer, 0, bytesRead);
				}

				outputStream.close();
				inputStream.close();

			} else {
				log.addInfo("responseCoden Error ::::" + responseCode);
				responseState = MRRequest.STATE_EXCEPTION;
				msg_info = "FileTransferAgent: Erorr Sending FileTransferRequest from " + SystemServiceConst.FILETRANSFERAGENT + " to " + MRConst.PUBLISH_FILE_DOWNLOAD_REQUEST
						+ ", Error=file '" + saveFilePath + "' does no exist or empty ";
			}

			return responseState;
		}
	}
}

