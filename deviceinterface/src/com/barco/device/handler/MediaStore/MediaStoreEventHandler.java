package com.barco.device.handler.MediaStore;

import java.net.URLEncoder;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.StreamEntry;
import com.barco.device.handler.Layout.TeardownRequestHandler;
import com.barco.utils.FileUtils;
import com.barco.utils.MediaUtils;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class MediaStoreEventHandler
implements MessageHandlerI, ResponseHandlerI
{
	public enum MEDIA_STORE_EVENTS {FileEvent , NASEvent, DiskSpaceUpdateEvent, ExportEvent, ImportEvent, OffloadEvent, MediaFileConverterEvent, PurgeFileProgressEvent};
	@Override
	public void handleMessage(Message message, DeviceI device)
			throws Exception
			{

		Document eventDoc  =message.getMessageDoc();
		Element eventElement = XMLUtils.getElementXpath(eventDoc, "//MediaStoreEvent");
		String eventType = XMLUtils.getAttributeValueWithDefault(eventElement, "type", "");	   
		switch (MEDIA_STORE_EVENTS.valueOf(eventType)) {
		case FileEvent:	
			handleFileEvent(eventElement,device);
			break;
		case NASEvent:
			handleNASEvent(eventElement,message,device);
			break;
		case DiskSpaceUpdateEvent:
			handleDiskSpaceUpdateEvent(eventElement,device);
			break;
		case ExportEvent:
			handleExportEvent(eventElement,message, device);
			break;
		case ImportEvent:
			handleImportEvent(eventElement,message, device);
			break;
		case OffloadEvent:
			handleOffloadEvent(eventElement,message, device);
			break;
		case MediaFileConverterEvent:
			handleMediaFileConverterEvent(eventElement,message, device);
			break;
		case PurgeFileProgressEvent:
			handlePurgeFileprogressEvent(eventElement,message,device);
			break;
		default:		
			break;
		}

	}
	private void handlePurgeFileprogressEvent(Element eventElement,
			Message message, DeviceI device) throws Exception {
		System.out.println(eventElement.asXML());
		String dirId = XMLUtils.getAttributeValueWithDefault(eventElement, "dirId", "");
		String dirNID = XMLUtils.getAttributeValueWithDefault(eventElement, "dirNID", "");
		String dirPath = XMLUtils.getAttributeValueWithDefault(eventElement, "dirPath", "");		
	
		dirId=getDirID(dirId, dirNID, dirPath, device);
		String id =  XMLUtils.getAttributeValueWithDefault(eventElement, "id", "");
		device.getMediaStoreAgentImpl().stopPurgeFileProcess(id, dirId, dirNID);	
	}
	
	private void handleOffloadEvent(Element eventElement, Message message,
			DeviceI device) throws Exception {
		Document messageDoc = message.getMessageDoc();
		Element eventEl = XMLUtils.getElementXpath(messageDoc, "//MediaStoreEvent");
		String dirNID = XMLUtils.getAttributeValueWithDefault(eventEl, "dirNID", "");
		String fileNID = XMLUtils.getAttributeValueWithDefault(eventEl, "clientID", "");
		String fileUUID = XMLUtils.getAttributeValueWithDefault(eventEl, "fileUUID", "");
		String error = XMLUtils.getAttributeValueWithDefault(eventEl, "error", "");
		String state = XMLUtils.getAttributeValueWithDefault(eventEl, "state", "");
		int uploadstate =0 ;
		if (!MediaStoreServiceConst.MEDIA_FILE_NFS_ERROR_EVENT.equals(state)) {
			long delFileSize= device.getMediaStoreAgentImpl().getFileSize("",dirNID,  fileUUID);
			device.getLogger().info("MediaStoreEventHandler : handleMessage : handleOffloadEvent : delFileSize="+delFileSize);
			//TODO: Use the purge process to delete the file.
			Element fileList = DocumentHelper.createElement("FileList");
			Element fileElememt = DocumentHelper.createElement("File");
			XMLUtils.addAttribute(fileElememt, MediaStoreServiceConst.BY_ID_ATTRIBUTE_NAME, fileUUID);	
			XMLUtils.addAttribute(fileElememt, MediaStoreServiceConst.FILE_NID_ATTRIBUTE_NAME, fileNID);
			fileList.add(fileElememt);
		    device.getMediaStoreAgentImpl().startPurgeFileProcess("", dirNID,fileList.elements());
		
		    //device.getMediaStoreAgentImpl().deleteFile("", dirNID, fileNID, fileUUID, true);
		    
			uploadstate = 8;
		//	long sizeAdded =0;
		//	device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, sizeAdded, delFileSize, 0, true);
		}
		device.getLogger().info("MediaStoreEventHandler : handleMessage : handleOffloadEvent : uploadstate = "+uploadstate + " state ="+state + " error ="+error);
		Document publishOffloadFileRequestDoc = MediaStoreUtils.publishOffloadFileStatus(state,error,fileNID,uploadstate,dirNID);
		Message publishRequestMessage =  Message.createRequest(publishOffloadFileRequestDoc,device.getAppserverJID(),device.getMyJID());
		device.sendRequest(publishRequestMessage, publishOffloadFileRequestDoc, this);
	}
	private void handleImportEvent(Element eventElement, Message message,
			DeviceI device) {
	    device.getLogger().info("IMPORT Event came in!: eventelement= "+eventElement.asXML());
		
	}
	private void handleExportEvent(Element eventElement, Message message,
			DeviceI device) {
		// TODO Auto-generated method stub
		
	}
	public void handleDiskSpaceUpdateEvent(Element eventElement, DeviceI device) throws Exception {
		
		String dirId = XMLUtils.getAttributeValueWithDefault(eventElement, "dirId", "");
		String dirNID = XMLUtils.getAttributeValueWithDefault(eventElement, "dirNID", "");
		String dirPath = XMLUtils.getAttributeValueWithDefault(eventElement, "dirPath", "");		
	
		dirId=getDirID(dirId, dirNID, dirPath, device);
		
		boolean publishUpdate = Utils.getBooleanValue(XMLUtils.getAttributeValueWithDefault(eventElement, "publishUpdate", ""),false);
		
		long sizeDeltaAdded = Utils.getLongValue(XMLUtils.getAttributeValueWithDefault(eventElement, "sizeDeltaAdded", ""),0);
		long sizeDeltaDeleted = Utils.getLongValue(XMLUtils.getAttributeValueWithDefault(eventElement, "sizeDeltaDeleted", ""),0);
		
		device.getMediaStoreAgentImpl().diskSpaceUpdate(dirId, dirNID, sizeDeltaAdded, sizeDeltaDeleted, 0, publishUpdate);
		
		if(device.getMediaStoreAgentImpl().isDiskFreeSpaceReachesToQuotaLimit(dirId, dirNID)){
		
			List<StreamEntry> listOfStreams = device.getDeviceDB().getStreamCache().getStreamByDirPath(dirPath);
			device.getLogger().info("Disk Quota Exceeds for "+dirId +" stoping pvr and recording streams. found  "+listOfStreams.size());
			
			for(StreamEntry streamEntry:listOfStreams) {
				//update stream state as setup error
				String exceptionMessage = new ServiceException(MediaAgentI.DEST_ERROR, "Disk Quota Exceeded").toString();
				Element exceptionEl = XMLUtils.stringToElement(exceptionMessage);
	
				//TBR: Backward compatibility on SC
				exceptionEl.setName("Exception");
				Element errorCode  = DocumentHelper.createElement("Code");
				errorCode.setText(String.valueOf(MediaAgentI.DEST_ERROR));
				exceptionEl.add(errorCode);
				streamEntry.status.add(exceptionEl);
	
				streamEntry.setStatus(MediaAgentI.DEST_ERROR, device, true);
	
				TeardownRequestHandler handler = new TeardownRequestHandler();
				handler.teardownStream(device, streamEntry.streamNID);
			}
			
		}
	}
	
	private String getDirID(String dirId,String dirNID,String dirPath,DeviceI device) throws Exception {	
	
		if(dirId.isEmpty() && dirNID.isEmpty()) {
			//get directory id using dirPath
			dirId = device.getMediaStoreAgentImpl().getDirIDFromDirPath(dirPath);
		}
		
		return dirId;
		
	}
	private void handleNASEvent(Element eventElement, Message message,
			DeviceI device) {
	    
	    String dirName = XMLUtils.getAttributeValueWithDefault(eventElement, "dirName", "");
        String state = XMLUtils.getAttributeValueWithDefault(eventElement, "state", "");
	    if (state.equals(MediaStoreServiceConst.ONLINE)) {
            device.getMediaStoreAgentImpl().handleDirAvailable(dirName);
        } else if (state.equals(MediaStoreServiceConst.OFFLINE)) {
            device.getMediaStoreAgentImpl().handleDirUnavailable(dirName);
        } else {
            return;
        }
		
	}
	
	public void handleFileEvent(Element eventElement, DeviceI device) throws Exception{
		
		String fileUUID = XMLUtils.getAttributeValueWithDefault(eventElement, "fileUUID", "");
		String dirId = XMLUtils.getAttributeValueWithDefault(eventElement, "dirId", "");
		String dirNID = XMLUtils.getAttributeValueWithDefault(eventElement, "dirNID", "");
		String dirPath = XMLUtils.getAttributeValueWithDefault(eventElement, "dirPath", "");
		String playLength = XMLUtils.getAttributeValueWithDefault(eventElement, "playLength", "");
		String size = XMLUtils.getAttributeValueWithDefault(eventElement, "size", "");
		device.getLogger().info("Size received from file event : "+size+" on dirPath: "+dirPath+" Having playlength: "+playLength);
		
		//Fix actual file size from blob area VNG0001-4071
		/*try{
			Long actualsize = device.getMediaStoreAgentImpl().getFileSize("", dirNID, fileUUID);
			device.getLogger().info("Size received from file event : "+size+ "Actual file size from blob area size : "+actualsize);
			
			if(size.isEmpty() || (actualsize !=0 && actualsize!=Utils.getLongValue(size,0))){
				size=actualsize.toString();
			}
		 }catch(Exception ex){
			ex.printStackTrace();
		}*/
		
		String state = XMLUtils.getAttributeValueWithDefault(eventElement, "state", "");
		device.getLogger().info("File state from file event : "+state);
		
		dirId=getDirID(dirId, dirNID, dirPath, device);
		
		device.getMediaStoreAgentImpl().setFilePlaylengthAndSize(dirId, dirNID, fileUUID,state, Utils.getLongValue(size,0), Utils.getLongValue(playLength, 0));
		
	}
	
	private void handleMediaFileConverterEvent(Element eventElement, Message message,
			DeviceI device) throws Exception {	
	    
		String type = eventElement.attributeValue("state");

		if("MediaFilePrepareComplete".equals(type) || "MediaFilePrepareError".equals(type)) {
			handlePrepareFileEvent(eventElement, message, device);
		} else if("MediaFileCreateCutlistComplete".equals(type) || "MediaFileCreateCutlistError".equals(type)) {
			handleCreateCutlistEvent(eventElement, message, device);
		}else if("ClearDownloadCacheComplete".equals(type) || "ClearDownloadCacheError".equals(type)) {
			handleClearDownloadCacheEvent(eventElement, message, device);
		}else if("MediaIngestionComplete".equals(type) || "MediaIngestionError".equals(type)) {
			handleMediaIngestion(eventElement, message, device);
		}else if("TrimMediaComplete".equals(type) || "TrimMediaError".equals(type)) {
			handleTrimMediaEvent(eventElement, message, device);
		}else if("MediaFileUnPackComplete".equals(type) || ("MediaFileUnPackError").equals(type)) {
			handleUnpackCompleteEvent(eventElement, message, device);
		}
	}
	
	private void handlePrepareFileEvent(Element eventElement, Message message,
			DeviceI device) throws Exception {
		String dirNID = eventElement.attributeValue("dirNID");
		String fileUUID = eventElement.attributeValue("fileUUID");
		String mediaFormat = eventElement.attributeValue("mediaFormat");
		String error = eventElement.attributeValue("error");
		String typeOfRequest = eventElement.attributeValue("typeOfRequest");
		String prepareFileName = eventElement.attributeValue("prepareFileName");
		long sizeUnreserved = Utils.getLongValue(eventElement.attributeValue("size"), 0);
		Document mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument("", dirNID, fileUUID);
		String state = "Prepared";
		if(!error.isEmpty())
			state = "Error";
		MediaStoreUtils.publishPrepareFileStatus(dirNID, fileUUID, mediaFormat, state, "Ready", mediaStoreFileDocument, device, this, typeOfRequest, prepareFileName);	
		device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, 0, 0, sizeUnreserved, true);
	}
	
	private void handleCreateCutlistEvent(Element eventElement, Message message,
			DeviceI device) throws Exception {
		String dirNID = eventElement.attributeValue("dirNID");
		String fileUUID = eventElement.attributeValue("fileUUID");
		String error = eventElement.attributeValue("error");
		String typeOfRequest = eventElement.attributeValue("typeOfRequest");
		String prepareFileName = eventElement.attributeValue("prepareFileName");
		long sizeUnreserved = Utils.getLongValue(eventElement.attributeValue("size"), 0);
		Document mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument("", dirNID, fileUUID);
		Long size = device.getMediaStoreAgentImpl().getFileSize("", dirNID, fileUUID);
		String state = "Ready";
		if(!error.isEmpty())
			state = "Error";
		XMLUtils.setValueXpath(mediaStoreFileDocument, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
		device.getMediaStoreAgentImpl().setFileRequest("", dirNID, "", fileUUID, mediaStoreFileDocument);
		MediaStoreUtils.sendPublishToSC(dirNID, fileUUID, "", true, size, mediaStoreFileDocument, null, device, this, device.getMediaStoreAgentImpl().getMediaStoreBookMarks("", dirNID, fileUUID), typeOfRequest, prepareFileName, "PublishFileRequest", null);
		device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, 0, 0, sizeUnreserved, true);
	}
	
	private void handleClearDownloadCacheEvent(Element eventElement, Message message,
			DeviceI device) throws Exception {
		String clientID = eventElement.attributeValue("clientID");
		String dirNID = eventElement.attributeValue("dirNID");
		String fileUUID = eventElement.attributeValue("fileUUID");
		Document mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument("", dirNID, fileUUID);
		Long size = device.getMediaStoreAgentImpl().getFileSize("", dirNID, fileUUID);
		MediaStoreUtils.sendPublishToSC(dirNID, fileUUID, clientID, false, size, mediaStoreFileDocument, null, device, this, null, "ClearCache", null, "PublishPrepareFileStatusRequest", null);
		device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, 0, 0, 0, true);
	}
	
	private void handleMediaIngestion(Element eventElement, Message message,
			DeviceI device) throws Exception {
		String dirNID = eventElement.attributeValue("dirNID");
		String fileUUID = eventElement.attributeValue("fileUUID");
		String sdpFilePath = eventElement.attributeValue("clientID");
		String error = eventElement.attributeValue("error");
		Document mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument("", dirNID, fileUUID);
		
		String state = "Ready";
		String dwnldstate = "Prepared";
		if(!error.isEmpty()){
			state = "Error";
			dwnldstate = "Error";
		}
		XMLUtils.setValueXpath(mediaStoreFileDocument, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
		
		//update sdp template info
		String sdpContent = FileUtils.readFile(sdpFilePath);
		if(!sdpContent.isEmpty()){
			sdpContent = MediaUtils.tokenizeSDP(sdpContent, device.getLogger());
			sdpContent = URLEncoder.encode(sdpContent, "UTF-8");
			XMLUtils.setValueXpath(mediaStoreFileDocument, "//SDP-Template-Src", sdpContent);
		}
		
		//Delete Temp SDP File
		FileUtils.deleteFileWithPath("", sdpFilePath);
		
		Element downloadEl = DocumentHelper.createElement("Download");
		Element mediaEl = DocumentHelper.createElement("Media");
		mediaEl.addAttribute("format", "Standard");
		mediaEl.addAttribute("state", dwnldstate);
		mediaEl.addAttribute("stale", String.valueOf(false));
		downloadEl.add((Element)mediaEl.detach());
		device.getMediaStoreAgentImpl().setFileRequest("", dirNID, "", fileUUID, mediaStoreFileDocument);
		//Delete EmptyStaleFile from blob
		String stalepath ="blob/"+fileUUID+"/standard/EmptyStaleFile";
		String  dirPath = device.getMediaStoreAgentImpl().getDirPath(dirNID, "");
		FileUtils.deleteFileWithPath(dirPath, stalepath);
		Long size = device.getMediaStoreAgentImpl().getFileSize("", dirNID, fileUUID);
		MediaStoreUtils.sendPublishToSC(dirNID, fileUUID, "", true, size, mediaStoreFileDocument, downloadEl, device, this, null, "", null, "PublishFileRequest", null);
		
		device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, 0, 0, 0, true);
	}

	private void handleTrimMediaEvent(Element eventElement, Message message,
			DeviceI device) throws Exception {
		String dirNID = eventElement.attributeValue("dirNID");
		String fileUUID = eventElement.attributeValue("fileUUID");
		String error = eventElement.attributeValue("error");
		String typeOfRequest = eventElement.attributeValue("typeOfRequest");
		long sizeUnreserved = Utils.getLongValue(eventElement.attributeValue("size"), 0);
		Document mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument("", dirNID, fileUUID);
		Long size = device.getMediaStoreAgentImpl().getFileSize("", dirNID, fileUUID);
		String state = "Ready";
		if(!error.isEmpty())
			state = "Error";
		XMLUtils.setValueXpath(mediaStoreFileDocument, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
		device.getMediaStoreAgentImpl().setFileRequest("", dirNID, "", fileUUID, mediaStoreFileDocument);
		
		//publish delete Bookmarks Element , if exist
		String deleteBookmarkListStr=FileUtils.readFileWithPath(MediaStoreUtils.PREPARE_TEMP_FOLDER_PATH+"/"+fileUUID+"/", fileUUID+".xml");
		Element deleteBookmarlListEl = null;
		if(deleteBookmarkListStr !=null && !deleteBookmarkListStr.isEmpty()){
			deleteBookmarlListEl = XMLUtils.stringToElement(deleteBookmarkListStr.trim());
		}
		//delete tmp file
		FileUtils.removeFileOrDir(MediaStoreUtils.PREPARE_TEMP_FOLDER_PATH+"/"+fileUUID+"/");
		
		//Publish download Element with NotReady status
		Element downloadEl = DocumentHelper.createElement("Download");
		Element mediaEl = MediaStoreUtils.createMediaDownloadElement("", "NotReady", false);
		downloadEl.add((Element)mediaEl.detach());
		MediaStoreUtils.sendPublishToSC(dirNID, fileUUID, "", false, size, mediaStoreFileDocument, downloadEl, device, this, device.getMediaStoreAgentImpl().getMediaStoreBookMarks("", dirNID, fileUUID), typeOfRequest, null, "PublishFileRequest", deleteBookmarlListEl);
		device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, 0, 0, sizeUnreserved, true);
	}
	
	private void handleUnpackCompleteEvent(Element eventElement, Message message,
		DeviceI device) throws Exception {	    
	    
	    String tarPath = eventElement.attributeValue("clientID");
	    String dirNID = eventElement.attributeValue("dirNID");
	    String fileUUID = eventElement.attributeValue("fileUUID");
	    String queueID = eventElement.attributeValue("queueID");
	    String groupFileUUID = eventElement.attributeValue("groupFileUUID");
	    String isGroupFilePresent = eventElement.attributeValue("isGroupFilePresent");
	    String error = eventElement.attributeValue("error");
	    MediaStoreAgentI mediaStoreAgent = device.getMediaStoreAgentImpl();   
	    Document mediaStoreFileDoc = mediaStoreAgent.getMediaStoreFileDocument("", dirNID, fileUUID);	   	    
	    String state = "Ready";
	    if(!error.isEmpty())
		state = "Error";
	    XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
	    Long size = mediaStoreAgent.getFileSize("", dirNID, fileUUID);	      
	    mediaStoreAgent.setFileRequest("", dirNID, "" , fileUUID, mediaStoreFileDoc);
	    if("false".equals(isGroupFilePresent)){
			Document mediaStoreGroupFileDoc = mediaStoreAgent.getMediaStoreFileDocument("", dirNID, groupFileUUID);
			MediaStoreUtils.sendPublishFileRequestToSC(dirNID, groupFileUUID,"", false, true, size, mediaStoreGroupFileDoc, null, device, this, mediaStoreAgent.getMediaStoreBookMarks("", dirNID, groupFileUUID), null, null);
	    }
	    MediaStoreUtils.sendPublishFileRequestToSC(dirNID, fileUUID,queueID, true, true, size, mediaStoreFileDoc, null, device, this, mediaStoreAgent.getMediaStoreBookMarks("", dirNID, fileUUID), null, null);
	    MediaStoreUtils.cleanupImportTempFiles(tarPath,fileUUID);	 
	    
	    device.getMediaStoreAgentImpl().diskSpaceUpdate("", dirNID, 0, 0, 0, true);
	}

	@Override
	public void handleResponse(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}
	@Override
	public void handleError(Message response, Document context, DeviceI device)
			throws Exception {
		// TODO Auto-generated method stub
		
	}


}
