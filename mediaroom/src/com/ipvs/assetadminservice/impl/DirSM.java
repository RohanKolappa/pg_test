package com.ipvs.assetadminservice.impl;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.FileUtils;

public class DirSM {
	
	// States
	public static final String READY = "READY";
	public static final String GETTING_SEND_PATH = "GETTING_SEND_PATH";
	public static final String EXTRACTING = "EXTRACTING";
	public static final String SENDING_FILE = "SENDING_FILE";
	public static final String DETACHING = "DETACHING";
	public static final String DETACHED = "DETACHED";
	// States
	public static final String GETTING_RECEIVE_PATH = "GETTING_RECEIVE_PATH";
	public static final String RECEIVING_FILE = "RECEIVING_FILE";
	
	// Transitions
	public static final String DETACH_REQUEST_OK = "DETACH_REQUEST_OK";
	public static final String GET_SEND_PATH_OK = "GET_SEND_PATH_OK";
	public static final String GET_SEND_PATH_ERROR = "GET_SEND_PATH_ERROR";
	public static final String EXTRACT_OK = "EXTRACT_OK";
	public static final String EXTRACT_ERROR = "EXTRACT_ERROR";
	public static final String SEND_FILE_OK = "SEND_FILE_OK";
	public static final String SEND_FILE_ERROR = "SEND_FILE_ERROR";
	public static final String DETACH_OK = "DETACH_OK";
	public static final String DETACH_ERROR = "DETACH_ERROR";
	// Transitions
	public static final String ATTACH_REQUEST_OK = "ATTACH_REQUEST_OK";
	public static final String GET_RECEIVE_PATH_OK = "GET_RECEIVE_PATH_OK";
	public static final String GET_RECEIVE_PATH_ERROR = "GET_RECEIVE_PATH_ERROR";
	public static final String RECEIVE_FILE_OK = "RECEIVE_FILE_OK";
	public static final String RECEIVE_FILE_ERROR = "RECEIVE_FILE_ERROR";
	public static final String UPDATE_OK = "UPDATE_OK";
	public static final String UPDATE_ERROR = "UPDATE_ERROR";
	public static final String ATTACH_OK = "ATTACH_OK";
	public static final String ATTACH_ERROR = "ATTACH_ERROR";

	// Actions
	public static final String GET_SEND_PATH = "GET_SEND_PATH";
	public static final String EXTRACT = "EXTRACT";
	public static final String SEND_FILE = "SEND_FILE";
	public static final String DETACH = "DETACH";
	public static final String SEND_DETACH_RESPONSE = "SEND_DETACH_RESPONSE";
	public static final String SEND_DETACH_ERROR = "SEND_DETACH_ERROR";
	// Actions
	public static final String GET_RECEIVE_PATH = "GET_RECEIVE_PATH";
	public static final String RECEIVE_FILE = "RECEIVE_FILE";
	public static final String UPDATE = "UPDATE";
	public static final String SEND_ATTACH_RESPONSE = "SEND_ATTACH_RESPONSE";
	public static final String SEND_ATTACH_ERROR = "SEND_ATTACH_ERROR";

    public static final String DIR_STATEMACHINE_XML = "/com/ipvs/assetadminservice/impl/DirStateMachineTransitions.xml";
    public static final int DB_SCRIPT_TIMEOUT = 6000;

    MRRequest request;
	XMPPI xmpp;
	MRLog log;
	MRDB db;
	MRResourceFactoryI resourceFactory;
	ServiceAgentFactoryI serviceAgentFactory;
	MRResponseHandlerI responseHandler;
	String state;
	String action;
	String dirNID;
	String dirAgentJID;
	String localExportPath;
	String localExportFilename;
	String localImportPath;
	String localImportFilename;
	String remoteFilename;
	String responseHandlerName;
	String lastTransition;
	static Document gDirStateMachine = null;
	boolean test = false;
		
	public static DirSM getDirSM(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			MRResponseHandlerI responseHandler,
			String dirNID, String state, String dirAgentJID, String remoteFilename,
			String responseHandlerName) throws Exception {
		DirSM dirSM = new DirSM();
		dirSM.request = userRequest;
		dirSM.xmpp = xmpp;
		dirSM.log = log;
		dirSM.db = db;
		dirSM.resourceFactory = resourceFactory;
		dirSM.serviceAgentFactory = serviceAgentFactory;
		dirSM.responseHandler = responseHandler;
		dirSM.dirNID = dirNID;
		dirSM.state = state;
		dirSM.dirAgentJID = dirAgentJID;
		dirSM.localExportPath = IPVSConst.getInstance().getIpvsDbExportDir();
		dirSM.localExportFilename = dirSM.localExportPath + "/" + dirNID + ".tar";
		dirSM.localImportPath = IPVSConst.getInstance().getIpvsDbImportDir();
		dirSM.localImportFilename = dirSM.localImportPath + "/" + dirNID + ".tar";
		dirSM.remoteFilename = remoteFilename;
		dirSM.responseHandlerName = responseHandlerName;
		dirSM.lastTransition = "";
		return dirSM;
	}
	
    public static final Document getDirStateMachine(MRLog log) throws Exception {
        if (gDirStateMachine == null) {
            gDirStateMachine = MRXMLUtils.loadXMLResource(DirSM.DIR_STATEMACHINE_XML, log);

            // System.out.println(MRXMLUtils.documentToString(gTrackStateMachine));
        }

        return gDirStateMachine;
    }

    public static String getDirAgentJID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String dirNID) throws Exception {
    	// Add the dirAgentJID to the requestData
        Document mediaDirDoc = null;
        try {
        	mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
         }
        catch(Exception e) {
    		throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "<dirNID>" + dirNID + "</dirNID>");
        }
        Element mediaStorePortEL = null;
        try {
            mediaStorePortEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log, false);
        }
        catch(Exception e) {
    		throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_AGENT_MISSING, "<dirNID>" + dirNID + "</dirNID>");
        }
        if(mediaStorePortEL == null) {
    		throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_AGENT_MISSING, "<dirNID>" + dirNID + "</dirNID>");
        }
        String mediaStorePortNID = MRXMLUtils.getValue(mediaStorePortEL);
        String dirAgentJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortNID);    	
        return dirAgentJID;
    }

    public static String getDirState(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String dirNID) throws Exception {
        Document mediaDirDoc = null;
        try {
        	mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
        }
        catch(Exception e) {
    		throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "<dirNID>" + dirNID + "</dirNID>");
        }
        String state = MRXMLUtils.getValueXpath(mediaDirDoc, "//State/StateFlag", "");
        return state;
	}

	public void setDirState(String state, String subState) throws Exception {
		log.addInfo("DirSM:setDirState state=" + state + " subState=" + subState +
				" dirNID=" + dirNID, MRLog.OK, MRLog.NOTIFY);
		MRRequest dirStateRequest = MRRequest.createRequest(request.getUserJID());
        Document mediaStreamDirDoc = MRXMLDBUtils.readElement(dirStateRequest, log, db, dirNID);
        MediaStoreServiceUtils.updateMediaStreamDirResourceStateAndMsg(mediaStreamDirDoc, state, subState, dirStateRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	//////////////////////////////////////////////
	// Private methods used by the action handlers
	//////////////////////////////////////////////

	void handleGetExportPath() throws Exception {
    	
    	// Send the getDirFTPPath request to the MediaStoreAdminServerAgent
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentJID = dirAgentJID;
        String toAgentName = MRConst.MEDIA_STORE_ADMIN_SERVICE;
        Element data = MediaRoomServiceUtils.cloneDataElement(request);

        String dataStr = MRXMLUtils.elementToString(data);
        String responseHandlerName = this.responseHandlerName;
        AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
        		fromAgentName, toAgentJID, toAgentName, dataStr, responseHandlerName);
    }

	private void handleSendError(int code, String dirState, String subState) throws Exception {
	   request.setState(MRRequest.STATE_EXCEPTION);
	   MRException exception = new MRException(code, this.lastTransition);
	   request.setException(exception);
       this.setDirState(dirState, subState);
       responseHandler.handleResponse(request, "");
       this.teardownExtractFile(log, localExportFilename);
	}

	private void handleSendResponse(String dirState, String subState) throws Exception {
	   request.setResult(MRXMLUtils.stringToDocument("<MediaStreamDirNID>" + dirNID + "</MediaStreamDirNID>"));
       this.setDirState(dirState, subState);
       responseHandler.handleResponse(request, "");
	}
			
	private void handleStartTransfer(String fromAgentJID, String fromFilename, String toAgentJID, String toFilename) throws Exception {
	   
	   String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
	   String toAgentName = SystemServiceConst.FILETRANSFERAGENT;
	   String responseHandlerName = this.responseHandlerName;
	    
	   Document requestDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.FILEXFERSETUPREQUESTFILE, log);
	   MRXMLUtils.setValueXpath(requestDoc, "//fromAgentJID", fromAgentJID);
	   MRXMLUtils.setValueXpath(requestDoc, "//fromFileName", fromFilename);
	   MRXMLUtils.setValueXpath(requestDoc, "//FileXferID",  dirNID);
	   MRXMLUtils.setValueXpath(requestDoc, "//toAgentJID", toAgentJID);
	   MRXMLUtils.setValueXpath(requestDoc, "//toFileName", toFilename);
	   String requestName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
	   
	   MRRequest ftpRequest = MRRequest.createRequest(request, requestDoc, requestName, request.getServiceName());	  	   
	   Element data = MRXMLUtils.getElementXpath(requestDoc, ftpRequest.getRequestDataXPath());
	   String dataStr = MRXMLUtils.elementToString(data);
	  
       String info = "DirSM::sendStartTransfer " + 
        			fromAgentName + " to " + toAgentName;
	   String serviceName = MRConst.FTP_SERVICE;
	    // String requestName = getOffsetRequest.getRequestName();
	   Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
	   String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
	   ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, toAgentName, requestName, dataStr,
	    		info, request.getNID(), request.getUserJID(), serviceName, clientData);
	}

	private void handleStopTransfer(String toAgentJID) throws Exception {
	   
	   String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
	   String toAgentName = SystemServiceConst.FILETRANSFERAGENT;
	   String responseHandlerName = this.responseHandlerName;
	    
	   Document requestDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.FILEXFERSTOPREQUESTFILE, log);
	   MRXMLUtils.setValueXpath(requestDoc, "//FileXferID",  dirNID);
	   String requestName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
	   MRRequest ftpRequest = MRRequest.createRequest(request, requestDoc, requestName, request.getServiceName());	  	   
	   Element data = MRXMLUtils.getElementXpath(requestDoc, ftpRequest.getRequestDataXPath());
	   String dataStr = MRXMLUtils.elementToString(data);
	  
       String info = "DirSM::sendStopTransfer " + 
        			fromAgentName + " to " + toAgentName;
	   String serviceName = MRConst.FTP_SERVICE;
	    // String requestName = getOffsetRequest.getRequestName();
	   Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
	   String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
	   ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, toAgentName, requestName, dataStr,
	    		info, request.getNID(), request.getUserJID(), serviceName, clientData);
	}

	private void teardownExtractFile(MRLog log, String filename) throws Exception {
		if(filename == null)
			return;
		File file = new File(filename);
		if(file.exists()) { // In case of a local transfer the file will not exist
			file.delete();
		}
		log.addInfo("DirSM:teardownExtractFile filename=" + filename, MRLog.OK, MRLog.NOTIFY);
	}

	///////////////////////////////
	// Action handler methods
	//////////////////////////////


    void handleGetSendPath() throws Exception {
    	this.handleGetExportPath();
    }

    void handleGetReceivePath() throws Exception {
    	this.handleGetExportPath();
    }

	public void handleSendDetachError() throws Exception {
		this.handleSendError(AMExceptionCodes.DETACH_DIR_ERROR, MediaStoreServiceConst.READY,
				DirSM.DETACH_ERROR);
		this.handleDeleteRemoteFile();
	}

	public void handleSendDetachResponse() throws Exception {
		this.handleSendResponse(MediaStoreServiceConst.DETACHED, DirSM.DETACHED);
	}

	public void handleSendAttachError() throws Exception {
		this.handleSendError(AMExceptionCodes.ATTACH_DIR_ERROR, MediaStoreServiceConst.DETACHED,
				DirSM.ATTACH_ERROR);
	}

	public void handleSendAttachResponse() throws Exception {
		this.handleSendResponse(MediaStoreServiceConst.READY, DirSM.READY);
		this.handleDeleteRemoteFile();
	}

	private void handleDeleteRemoteFile() throws Exception {
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
    	String toAgentJID = this.dirAgentJID;
        String toServiceName = MRConst.MEDIA_STORE_ADMIN_SERVICE;
        Element data = MediaRoomServiceUtils.cloneDataElement(request);
        MRXMLUtils.getElementXpath(data, ".//MediaStreamDirNID").setText(this.dirNID);
        Element dirState = MRXMLUtils.stringToElement("<dirState>" + 
        		MediaStoreServiceConst.READY + "</dirState>");
        data.add(dirState);

        String dataStr = MRXMLUtils.elementToString(data);
        // System.out.println("DirSM:handleDeleteRemoteFile dirAgentJID=" + dirAgentJID + " dataStr=" + dataStr);
        String responseHandlerName = this.responseHandlerName;
        AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
        		fromAgentName, toAgentJID, toServiceName, dataStr, responseHandlerName);
    }

	// Renames "from" file to "to" file using copy and delete.
	boolean renameTo(File from, File to) {
		try {
			// Copy the file from - to
		    InputStream in = new FileInputStream(from);
		    OutputStream out = new FileOutputStream(to);
	
		    // Transfer bytes from in to out
		    byte[] buf = new byte[1024];
		    int len;
		    while ((len = in.read(buf)) > 0) {
		        out.write(buf, 0, len);
		    }
		    in.close();
		    out.close();
		    
		    // Delete the from file
		    from.delete();
		}
		catch(Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	public void handleSendLocalFile() throws Exception {
		File localFile = new File(localExportFilename);
		File remoteFile = new File(remoteFilename);
		if(!renameTo(localFile, remoteFile))
			throw new MRException(AMExceptionCodes.SEND_FILE_LOCAL_FAILED, "Send File Local Failed");
	}

	public void handleReceiveLocalFile() throws Exception {
		File localFile = new File(localImportFilename);
		File remoteFile = new File(remoteFilename);
		if(!renameTo(remoteFile, localFile))
			throw new MRException(AMExceptionCodes.RECEIVE_FILE_LOCAL_FAILED, "Receive File Local Failed");
	}


	public void handleSendFile() throws Exception {
		String localAgentJID = request.getAppServerJID();
		this.handleStartTransfer(localAgentJID, localExportFilename, dirAgentJID, remoteFilename);
	}

	public void handleReceiveFile() throws Exception {
		String localAgentJID = request.getAppServerJID();
		this.handleStartTransfer(dirAgentJID, remoteFilename, localAgentJID, localImportFilename);
	}

	public void handleExtract() throws Exception {
        String exportParams = "<"+AMConst.EXPORT_MEDIA_DIR+">" +
        						"<params>" +
									"<mediaStreamDirResourceNID>"+dirNID+"</mediaStreamDirResourceNID>" +
									"<zipFolderPath>"+this.localExportPath+ "</zipFolderPath>" +
								"</params>" +
							   "</"+AMConst.EXPORT_MEDIA_DIR+">";
	    long timeout =DB_SCRIPT_TIMEOUT;
		String os = IPVSConst.getInstance().getOS();
		if("windows".equals(os)) {
			FileUtils.writeFile(this.localExportFilename, exportParams);
		}
		else {
		    MRXMLDBUtils.exportDB(timeout, request, db, log, AMConst.EXPORT_MEDIA_DIR, exportParams);
		}
		log.addInfo("DirSM:handleExtract filename=" + this.localExportFilename, MRLog.OK, MRLog.NOTIFY);
	}

	public void handleUpdate() throws Exception {
		String localAgentJID = request.getAppServerJID();
		this.handleStopTransfer(localAgentJID);
        String importParams = "<"+AMConst.IMPORT_MEDIA_DIR+">" +
        						"<params>" +
									"<mediaStreamDirResourceNID>"+dirNID+"</mediaStreamDirResourceNID>" +
							        "<zipFolderPath>"+this.localImportPath+ "</zipFolderPath>" +
								"</params>" +
							  "</"+AMConst.IMPORT_MEDIA_DIR+">";
	    long timeout =DB_SCRIPT_TIMEOUT;
		String os = IPVSConst.getInstance().getOS();
		if("windows".equals(os)) {
		}
		else {
			if(test) {
		        System.out.println("DirSM:handleUpdate skipping importDB test=" + test);
			}
			else {
			    MRXMLDBUtils.importDB(timeout, request, db, log, AMConst.IMPORT_MEDIA_DIR , importParams);
			}
		}
		log.addInfo("DirSM:handleUpdate filename=" + this.localImportFilename, MRLog.OK, MRLog.NOTIFY);
	}

	// Use in attach
	private void handleDetach() throws Exception {
		this.handleStopTransfer(this.dirAgentJID);
	    String deleteParams = "<"+AMConst.DELETE_MEDIA_DIR+">" +
	    						"<params>" +
									"<mediaStreamDirResourceNID>"+dirNID+"</mediaStreamDirResourceNID>" +
								"</params>" +
							  "</"+AMConst.DELETE_MEDIA_DIR+">";
	    long timeout =DB_SCRIPT_TIMEOUT;
		String os = IPVSConst.getInstance().getOS();
		if("windows".equals(os)) {
		}
		else {
			if(test) {
		        System.out.println("DirSM:handleExtract skipping deleteDB test=" + test);
			}
			else {
			    MRXMLDBUtils.deleteDB(timeout, request, db, log, AMConst.DELETE_MEDIA_DIR, deleteParams);
			}
		}
		this.teardownExtractFile(log, this.localExportFilename);
		log.addInfo("DirSM:handleDetach filename=" + this.localExportFilename, MRLog.OK, MRLog.NOTIFY);
	}
	
	public void handleTransition(String transition) throws Exception {
		while(transition != null) {
			transition = this.handleTransitionInternal(transition);
		}
	}

	public String handleTransitionInternal(String transition) throws Exception {
		this.lastTransition = transition;
        String nextTransition = null;
		this.action = null;
        Document dirStateMachineDoc = DirSM.getDirStateMachine(log);
        Element transitionElement = MRXMLUtils.getElementXpath(dirStateMachineDoc, "//Transition[@beginstate='" + state + "' and @transition='" + transition + "']");

        if (transitionElement == null) {
        	System.out.println("DirSM:handleTransitionInternal state=" + this.state + " transition=" + transition + " action=" + action);
            throw new MRException(AMExceptionCodes.DIR_TRANSITION_NOT_FOUND,
            		MRXMLUtils.generateErrorXML("transition", transition) );
        }
        
        state = transitionElement.attributeValue("endstate");
        action = transitionElement.attributeValue("action");
        this.setDirState(MediaStoreServiceConst.BUSY, state);
        
    	log.addInfo("DirSM:handleTransitionInternal state=" + this.state + " transition=" + transition + " action=" + action, MRLog.OK, MRLog.NOTIFY);

		//////////////////////////////////////////////////////
		// Detach Transitions
		/////////////////////////////////////////////////////
		if(DirSM.GET_SEND_PATH.equals(action))
			this.handleGetSendPath();
		else if(DirSM.EXTRACT.equals(action)) {
			try {
				this.handleExtract();
				nextTransition = DirSM.EXTRACT_OK;
			}
			catch(Exception e) {
				e.printStackTrace();
				nextTransition = DirSM.EXTRACT_ERROR;
			}
		}
		else if(DirSM.SEND_FILE.equals(action)) {
			String localAgentJID = request.getAppServerJID();
			if(localAgentJID.equals(dirAgentJID)) { // Local transfer just move the file
				try {
					this.handleSendLocalFile();
					nextTransition = DirSM.SEND_FILE_OK;
				}
				catch(Exception e) {
					System.out.println("DirSM:handleTransitionInternal from=" + localExportFilename + " to=" + remoteFilename);
					e.printStackTrace();
					nextTransition = DirSM.SEND_FILE_ERROR;
				}
			}
			else
				this.handleSendFile();
		}
		else if(DirSM.DETACH.equals(action)) {
			try {
				this.handleDetach();
				nextTransition = DirSM.DETACH_OK;
			}
			catch(Exception e) {
				e.printStackTrace();
				nextTransition = DirSM.DETACH_ERROR;
			}
		}
		else if(DirSM.SEND_DETACH_ERROR.equals(action))
			this.handleSendDetachError();
		else if(DirSM.SEND_DETACH_RESPONSE.equals(action))
			this.handleSendDetachResponse();
		
		//////////////////////////////////////////////////////
		// Attach Transitions
		/////////////////////////////////////////////////////
		else if(DirSM.GET_RECEIVE_PATH.equals(action))
			this.handleGetReceivePath();
		else if(DirSM.RECEIVE_FILE.equals(action)) {
			String localAgentJID = request.getAppServerJID();
			if(localAgentJID.equals(dirAgentJID)) { // Local transfer just move the file
				try {
					this.handleReceiveLocalFile();
					nextTransition = DirSM.RECEIVE_FILE_OK;
				}
				catch(Exception e) {
					System.out.println("DirSM:handleTransitionInternal from=" + remoteFilename + " to=" + localImportFilename);
					e.printStackTrace();
					nextTransition = DirSM.RECEIVE_FILE_ERROR;
				}
			}
			else
				this.handleReceiveFile();
		}
		else if(DirSM.UPDATE.equals(action)) {
			try {
				this.handleUpdate();
				nextTransition = DirSM.UPDATE_OK;
			}
			catch(Exception e) {
				e.printStackTrace();
				nextTransition = DirSM.UPDATE_ERROR;
			}
		}
		else if(DirSM.SEND_ATTACH_ERROR.equals(action))
			this.handleSendAttachError();
		else if(DirSM.SEND_ATTACH_RESPONSE.equals(action))
			this.handleSendAttachResponse();
		
		return nextTransition;
	}
	
}
