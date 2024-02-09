package com.ipvs.device.http.servlet;

import java.io.IOException;
import java.io.InputStream;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.eclipse.jetty.server.Request;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.BaseManagementRequestHandler;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.http.server.DeviceHttpServerAgent;
import com.ipvs.http.common.HttpConst;
import com.ipvs.http.common.HttpExceptionCodes;
import com.ipvs.http.servlet.IPVSHttpServlet;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

@SuppressWarnings("unused")
public class DeviceSnapshotServlet extends IPVSHttpServlet{

	private static final long serialVersionUID = 1L;

	protected DeviceHttpServerAgent httpServerAgent = null; 
	private final String REQUEST_STATE = "REQUEST_STATE";
	private final int WAITING_FOR_RESPONSE_FROM_APPSERVER = 1;
	private final int DONE = 200;
	public DeviceSnapshotServlet(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			String userJID,DeviceHttpServerAgent httpServerAgent) {
		super(xf, log, db, resourceFactory, userJID);
		this.httpServerAgent = httpServerAgent;
	}	


	protected void doPost(HttpServletRequest req, HttpServletResponse resp)
	throws ServletException, IOException {
		InputStream is = req.getInputStream();		
		
		Continuation cc = ContinuationSupport.getContinuation(req);	
		
	    //if response element is not empty then write result	
		if(req.getAttribute(RESULTS) !=null){
			log.addInfo("DeviceSnapshotServlet: RESPONSE received from appserver. ",MRLog.INFO,MRLog.NOTIFY);
			if((Integer)cc.getAttribute(REQUEST_STATE)==MRRequest.STATE_DONE) {
				resp.setStatus(HttpServletResponse.SC_CREATED);
			}  else {
				resp.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);	
			}
			resp.setContentType("text/xml");
			String url = (String)cc.getAttribute(RESULTS);
			url= url.replaceAll("&", "&amp;");			
			resp.getWriter().println("<Response><URL>"+url+"</URL><NID>"+cc.getAttribute(HttpConst.X_IPVS_MEDIANID)+"</NID></Response>");	
			resp.setHeader(HttpConst.X_IPVS_MEDIANID, (String)cc.getAttribute(HttpConst.X_IPVS_MEDIANID));
			((Request)req).setHandled(true);
			return;
		} else if(req.getAttribute(REQUEST_STATE)!=null && ((Integer)req.getAttribute(REQUEST_STATE)== WAITING_FOR_RESPONSE_FROM_APPSERVER)) {
			//if the request state is waiting for response from appserver put the thread again into suspend queue
			log.addInfo("DeviceSnapshotServlet: Waiting for response from appserver",MRLog.INFO,MRLog.NOTIFY);
			cc.setTimeout(CONTINUATION_TIME_OUT);
			cc.suspend();	
			return;
		}
			
		

		try {
			byte[] bytes = new byte[req.getContentLength()];
			is.read(bytes);
			String byteString = new String(bytes);

			Document createSnapshotDoc  = MRXMLUtils.stringToDocument(byteString);		
			MediaStoreAdminServerAgent storeAgent = (MediaStoreAdminServerAgent) xf.sf(userJID).getServiceAgent(MediaStoreAdminServerAgent.class.getName());

			//find directory info
			String dirId = MRXMLUtils.getAttributeValue(createSnapshotDoc.getRootElement(), HttpConst.DIR_ID_ATTRIBUTE, "");
			MediaStoreDirectoryInfo dirInfo = storeAgent.getMediaStoreDirectoryInfo(dirId);
			if(dirInfo == null) {
				throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NID_NOT_FOUND, "Dir Not Found. DirId:="+dirId);
			}
			
			//verify disk space reached to threshold or not
			storeAgent.checkAvailableDiskSpace(dirInfo.getMediaStoreDirNID());

			String mediaStoreFileNID = Utils.getUUID();
			//create media stream file document
			org.dom4j.Document mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
			Element metaDataEl = MRXMLUtils.getElementXpath(createSnapshotDoc, ".//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
			String metaData = Utils.decode(metaDataEl.asXML());
			Long now = xf.xmpp(userJID).getCurrentTimeMillis();

			MediaStoreServiceUtils.updateMediaStreamFileDocWithInfo(xf.xmpp(userJID), mediaStreamFileDoc, MediaStoreServiceConst.MEDIA_IMAGE_TYPE, 
					MRXMLUtils.getAttributeValue(createSnapshotDoc.getRootElement(), SessionServiceConst.TITLE, "snapshot"), "", 
					dirInfo.getMediaStoreDirNID(), null , mediaStoreFileNID, "", MediaStoreServiceConst.READY, MediaStoreServiceConst.TRUE, MRXMLUtils.stringToElement(metaData), String.valueOf(now),String.valueOf(now),"false", null);			

			MRRequest request = MRRequest.createRequest(userJID);
			new BaseManagementRequestHandler().customValidateReq("Add", MRConfig.getObjectServiceName(AMConst.MEDIA_STREAM_FILE_RESOURCE), AMConst.MEDIA_STREAM_FILE_RESOURCE, null, (Element)mediaStreamFileDoc.getRootElement().clone(), request, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
					       
			//update image type
			MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.IMAGE_TYPE_ELEMENT, MRXMLUtils.getAttributeValue(createSnapshotDoc.getRootElement(), HttpConst.IMAGE_TYPE_ATTRIBUTE,""));

			//create blob 
			MediaStoreServerI storeServer = storeAgent.getMediaStoreServer();
			String rootDir=dirInfo.getMediaStoreRootPath();
			int revision = storeServer.getNewRevisionNumber(rootDir);			
			storeServer.createMediaStoreFile(mediaStoreFileNID, revision, mediaStreamFileDoc, rootDir);

			//move file from temporary area to blob
			String filePath  = MRXMLUtils.getAttributeValue(createSnapshotDoc.getRootElement(),HttpConst.FILE_PATH_ATTRIBUTE,"");
			String storePath = rootDir +MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR +"/" + mediaStoreFileNID +"/";
		
			//find file path
			int pos = filePath.lastIndexOf('.');
			String extension = filePath.substring(pos);
			if(extension.equals(".jpeg"))
				extension = ".jpg";
			else if(extension.equals(".tiff"))
				extension = ".tif";
			String snapshotFilePath = storePath+MediaStoreServiceConst.SNAPSHOT_FILE_NAME+extension;
		
			//write INDEX file
			String indexFilePath = storePath + MediaStoreServiceConst.SS_INDEX_FILE_NAME;
			FileUtils.writeFile(indexFilePath, String.valueOf(now +" "+MediaStoreServiceConst.SNAPSHOT_FILE_NAME+extension));

			//move file from temporary area to blob area
			MediaRoomServiceUtils.executeSystemCommand(new String[]{"mv",filePath,snapshotFilePath});

			//create Thumbnail
			String [] thumbnailCmd = new String [] {MediaRoomServiceConst.FFCREATOR_TOOL, "-t", "-s", storePath, "-F", MediaRoomServiceConst.SNAPSHOT};
			MediaRoomServiceUtils.executeSystemCommand(thumbnailCmd);
			
			//Send message to appserver
			Document mediaStoreFileDoc = storeServer.getMediaStoreFileDoc(mediaStoreFileNID, rootDir);
			long size = storeServer.getMediaSize(mediaStoreFileNID, rootDir);
			String clientdata   = Utils.getUUID();
			MediaStoreServiceUtils.sendAddMediaStreamFileRequestToAppserver(dirInfo.getMediaStoreDirNID(), mediaStoreFileNID, MediaStoreServiceConst.TRUE, size, revision,
					SessionServiceResourceConst.SELECTFORMAT_BASICURL,  mediaStoreFileDoc, DeviceHttpServerAgent.class.getName(), xf.xmpp(userJID), db, log, indexFilePath, xf.sf(userJID),clientdata);

			
			cc.setAttribute(REQUEST_STATE, WAITING_FOR_RESPONSE_FROM_APPSERVER);
	
			//start continuation
			cc.setTimeout(CONTINUATION_TIME_OUT);
			cc.suspend();		
			httpServerAgent.addServlettoCache(clientdata, this,cc);			
			
			//update dir size used. 
			storeAgent.updateDirSize(dirInfo.getMediaStoreDirNID());
	
		} catch (MRException e) {
	    	e.printStackTrace();	
	    	resp.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
			resp.getWriter().println(e.toString());
			((Request)req).setHandled(true);	
		} catch (Exception e) {
			e.printStackTrace();
			resp.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
			resp.getWriter().println(new MRException(HttpExceptionCodes.REQUEST_FAILED, MRException.getStackTrace(e)).toString());
			((Request)req).setHandled(true);	
		}		
	}


	@Override
	public void handleAgentResponse(Document agentResponseDoc, Continuation cc) {
		String result = "";
		int state= MRRequest.STATE_DONE;
		String fileNID = "";
		try {
			String fileURL = MRXMLUtils.getAttributeValueXpath(agentResponseDoc,"//"+SessionServiceConst.FILE,SessionServiceResourceConst.MEDIAURL_ATTRIBUTE,"");
			if(fileURL.isEmpty()) {
				throw new MRException(HttpExceptionCodes.CREATING_SNAPSHOT_FAILED, "Creating Snapshot Failed");
			}
			fileNID = MRXMLUtils.getAttributeValueXpath(agentResponseDoc,"//"+SessionServiceConst.FILE,SessionServiceConst.NID,"");
			result = fileURL;
		} catch (Exception e) {
			result = MRException.getStackTrace(e);
			e.printStackTrace();
			state= MRRequest.STATE_EXCEPTION;
		}	
		//set results and resume thread
		cc.setAttribute(RESULTS, result);
		cc.setAttribute(REQUEST_STATE, state);
		cc.setAttribute(HttpConst.X_IPVS_MEDIANID,fileNID);
		cc.resume();
	}
}
