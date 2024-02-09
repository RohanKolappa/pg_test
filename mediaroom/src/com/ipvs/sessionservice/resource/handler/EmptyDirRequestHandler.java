package com.ipvs.sessionservice.resource.handler;

import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
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
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;

public class EmptyDirRequestHandler extends FileBaseRequestHandler
implements MRRequestHandlerI{

	private static Map<String, String> processState = new HashMap<String, String>();
	private static Map<String, List<String>> requestedUserList = new HashMap<String, List<String>>();
	public static final String ACTION_REQUEST_ELEMENT_NAME = "ActionRequest";
	public static final String ACTION_RESPONSE_ELEMENT_NAME = "ActionResponse";




	private void addUserTOList(String userJID, String dirNID) {
		List<String> listOfUsers = requestedUserList.get(dirNID);

		if ((listOfUsers == null) || listOfUsers.isEmpty()) {
			listOfUsers = new ArrayList<String>();
		} else {
			if (listOfUsers.contains(userJID)) {
				return;
			}
		}

		listOfUsers.add(userJID);
		requestedUserList.put(dirNID, listOfUsers);
	}

	//check state is started or not. If started then return false. if not the set the state and return true.
	private synchronized boolean setProcessState(String state, String dirNID, String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
			throws MRException, Exception {

		synchronized (processState) {				

			String processState = EmptyDirRequestHandler.processState.get(dirNID);

			if ((processState == null) && state.equals(AMConst.EMPTY_DIR_PROCESS_STARTED)) {
				EmptyDirRequestHandler.processState.put(dirNID, state);
				addUserTOList(userJID, dirNID);
			} else if (state.equals(AMConst.EMPTY_DIR_PROCESS_STOPPED)) {
				sendStoppedNotificationToUsers(AMConst.PROCESS_STOPPED, dirNID, xmpp, log, db, resourceFactory, profileFactory);
				handleEmptyMediaGroupForDeletedFiles(userJID,xmpp, log, db, resourceFactory, profileFactory);
				removeUserList(dirNID);
				EmptyDirRequestHandler.processState.remove(dirNID);
			} else if (AMConst.EMPTY_DIR_PROCESS_STARTED.equals(processState)) {
				addUserTOList(userJID, dirNID);

				return false;
			}

		}

		return true;
	}

	private  void sendStoppedNotificationToUsers(int eventCode, String dirNID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
			throws MRException, Exception {
		List<String> userList = requestedUserList.get(dirNID);
		System.out.println("userList:="+userList);
		if (userList == null) {
			return;
		}

		for (String userJID : userList) {
			String eventXML = AssetAdminServiceUtils.createEmptyRecycleBinEventMessage(dirNID, AMConst.EMPTY_DIR_ACTION, eventCode);
			String info = "EmptyDirProcess > AgentEvent Message action=" + AMConst.EMPTY_DIR_ACTION + " and EventCode:=" + eventCode + " to " + ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.OPERATOR);
			ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, userJID, MRConst.OPERATOR_SERVICE, AMConst.EMPTY_DIR_ACTION, MRXMLUtils.stringToElement(eventXML), info);
		}
	}

	private  void removeUserList(String dirNID) {
		requestedUserList.remove(dirNID);
	}

	private  synchronized String getProcessState(String dirNID) {
		return EmptyDirRequestHandler.processState.get(dirNID);
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		//TODO: Design API and then implement corresponding request handling
		return null;
	}

	public Document handleEmptyRecycleBinRequest(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String dirNID,String requestedAction)  throws Exception {
		if (dirNID.isEmpty()) {
			throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "MediaStreamDirNID Missing");
		}

		String processState = getProcessState(dirNID);
		log.addInfo("EmptyDirProcess ProcessState:=" + processState + " RequestedAction:=" + requestedAction, MRLog.INFO, MRLog.NOTIFY);

		boolean isActionValid = true;

		try {
			//if the requested state is start then change the state as started if the process not started.
			if (requestedAction.equals(AMConst.START_EMPTY_DIR_PROCESS)) {
				log.addInfo("Requested EmptyDir Start Process. userJID:=" + request.getUserJID(), MRLog.INFO, MRLog.NOTIFY);

				if (!setProcessState(AMConst.EMPTY_DIR_PROCESS_STARTED, dirNID, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory)) {
					return generateResponseDoc(request, dirNID, AMConst.EMPTY_DIR_PROCESS_STARTED);
				}

				// if the requested action is stop dir process then stop the process 
			} else if (requestedAction.equals(AMConst.STOP_EMPTY_DIR_PROCESS)) {
				log.addInfo("Requested EmptyDir Stop Process. userJID:=" + request.getUserJID(), MRLog.INFO, MRLog.NOTIFY);
				setProcessState(AMConst.EMPTY_DIR_PROCESS_STOPPED, dirNID, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory);

				return generateResponseDoc(request, dirNID, AMConst.EMPTY_DIR_PROCESS_STOPPED);
			} //if the request action is nothing and process state is stopped  then send no response.
			else if (requestedAction.isEmpty() && (processState == null)) {
				log.addInfo("Empty Dir Process Stopped", MRLog.INFO, MRLog.NOTIFY);

				return null;

				// if the requested action id not start/stop/empty then return exception
			} else if (!requestedAction.equals(AMConst.START_EMPTY_DIR_PROCESS) && !requestedAction.equals(AMConst.STOP_EMPTY_DIR_PROCESS) && !requestedAction.isEmpty()) {
				isActionValid = false;
				throw new MRException(AMExceptionCodes.INVALID_EMPTY_DIR_ACTION, "Invalid Empty Dir Action");
			}

			//send delete request to media store if the state is READY.
			boolean deletedAll= handleEmptyDirRequest(dirNID,request, db, log, xmpp, resourceFactory, serviceAgentFactory);

			if (!deletedAll) {
				setProcessState(AMConst.EMPTY_DIR_PROCESS_STOPPED, dirNID, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory);
			}

			MRXMLUtils.setValueXpath(request.getMessage().getDocument(), "//"+MRConst.CLIENT_DATA, request.getClientData());
		} catch (MRException exp) {
			exp.printStackTrace();

			if (!isActionValid) {
				throw new MRException(exp.getCode(), exp.getData());
			}

			setProcessState(AMConst.EMPTY_DIR_PROCESS_STOPPED, dirNID, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory);
			MRXMLUtils.setValueXpath(request.getMessage().getDocument(), "//"+MRConst.CLIENT_DATA, request.getClientData());

			return generateResponseDoc(request, dirNID, AMConst.EMPTY_DIR_PROCESS_STOPPED);
		} catch (Exception exp) {
			exp.printStackTrace();

			if (!isActionValid) {
				throw new MRException(AMExceptionCodes.EMPTRY_RECYCLE_BIN_REQUEST_FAILED, exp.toString());
			}

			setProcessState(AMConst.EMPTY_DIR_PROCESS_STOPPED, dirNID, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory);
			MRXMLUtils.setValueXpath(request.getMessage().getDocument(), "//"+MRConst.CLIENT_DATA, request.getClientData());

			return generateResponseDoc(request, dirNID, AMConst.EMPTY_DIR_PROCESS_STOPPED);
		}

		return generateResponseDoc(request, dirNID, AMConst.EMPTY_DIR_PROCESS_STARTED);
	}


	/**
	 * Get the list of recycle bin files and send message to media store agent to delete that first file in the list.
	 * before sending check the file state is ok or not and DMS user is up or not.
	 * @param request
	 * @param db
	 * @param log
	 * @param xmpp
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @return
	 * @throws MRException
	 * @throws Exception
	 */
	public boolean handleEmptyDirRequest(String dirNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {

		if (dirNID.isEmpty()) {
			throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "MediaStreamDirNID Missing");
		}


		String []whereXpath = {"//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+"[.='"+dirNID+"']", "//StateFlag[.!='"+MediaStoreServiceConst.BUSY+"']"};
		/*	String selectXML = "<MediaStreamFileResource NID='{$VAR/@NID/data(.)}'> " +
				"{$VAR//MediaStoreFileInfo/Type}" +
				"{$VAR//UUID}</MediaStreamFileResource>";*/
		Element responseEL = MRXMLUtils.newElement("ResponseEL");
		responseEL = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.MEDIA_STREAM_FILE_RESOURCE,
				null, null, whereXpath, null, 
				null, "", responseEL, 0, 1);
		List<?> mediaStreamFileList = MRXMLUtils.getList(responseEL);        

		if (mediaStreamFileList == null) {
			log.addInfo("No files found in dir.", MRLog.INFO, MRLog.NOTIFY);

			return false;
		}

		Iterator<?> mediaStreamfileListIterator = mediaStreamFileList.iterator();

		Element mediaStreamFileEL = null;
		Document mediaStreamFileDoc = null;
		
		while (mediaStreamfileListIterator.hasNext()) {
			try {
				mediaStreamFileEL = (Element) mediaStreamfileListIterator.next();
				mediaStreamFileDoc = MRXMLUtils.elementToDocument(mediaStreamFileEL);		
				MRRequest newRequest = MRRequest.createRequest(request, request.getMessage().getDocument(), request.getRequestName(), request.getServiceName(), dirNID);
				new DeleteFileRequestHandler().deleteFile(mediaStreamFileDoc, newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, com.ipvs.sessionservice.resource.handler.EmptyDirRequestHandler.class.getName());
				return true;
			} catch (MRException exp) {
				exp.printStackTrace();
			} catch (Exception exp) {
				exp.printStackTrace();
			}
		}
		//XMPPTransaction.setDEBUG(false,Thread.currentThread().getId());
		log.addInfo("Can not proceed Further. Either File state is Error or Store User is Offline", MRLog.INFO, MRLog.NOTIFY);

		return false;
	}


	private Document generateResponseDoc(MRRequest request, String dirNID, String actionResponse)
			throws MRException, Exception {
		Document responseDoc = null;
		String responseName = request.getResponseDataName();
		StringBuffer responseStr = new StringBuffer();
		responseStr.append("<" + responseName + ">");
		responseStr.append(MRXMLUtils.generateXML(ACTION_RESPONSE_ELEMENT_NAME, actionResponse));
		responseStr.append(MRXMLUtils.generateXML(AMConst.MEDIA_STREAM_DIR_NID, dirNID));
		responseStr.append("</" + responseName + ">");
		responseDoc = MRXMLUtils.stringToDocument(responseStr.toString());

		return responseDoc;
	}


	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception
	{
		Document responseDoc = request.getResult();
		String mediaStreamFileNID = "";
		try {
			MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, 9102);
			Element fileEl = MRXMLUtils.getElementXpath(responseDoc, "//File");
			mediaStreamFileNID = MRXMLUtils.getAttributeValue(fileEl, "NID", "");
			request.setSuperUser(true);
			new DeleteFileRequestHandler().deleteMediaStreamFileDB(mediaStreamFileNID, request, log, db, xmpp, resourceFactory, serviceAgentFactory,new EmptyDirRequestHandler());
			
			// invalidate the recording thumbnail cache after delete.
			MediaStoreServiceUtils.invalidateFileThumbnailCache(mediaStreamFileNID, log);
	        
		} catch (Exception exp) {
			exp.printStackTrace();
		}

		String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
		Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));

		//create user request
		MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);	
		if (userRequest == null) {
			return;
		}

		log.addInfo("DeletMediaStoreFileResponse: Call Empty Recycle Bin Handler to process with next file deletion or not.", MRLog.INFO, MRLog.NOTIFY);
		String dirNID = userRequest.getClientData();
		this.handleEmptyRecycleBinRequest(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, dirNID, "");

	}	

}
