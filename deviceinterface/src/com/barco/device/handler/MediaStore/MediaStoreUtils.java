package com.barco.device.handler.MediaStore;

import java.io.File;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;
import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class MediaStoreUtils {
    
	// Convertor tool
	public static final String SCRIPT_MEDIA_CONVERTOR_EXEC = "/home/onair/TV/bin/mediaconverter";
	
	// Prepare File Constants 
    public static final String PREPARE_TEMP_FOLDER_PATH = "/data/dms/mediafile/prepare"; 

    //unpack file constants 
    public static final String SCRIPT_MEDIA_FILE_CONVERSION = "/etc/scripts/mediafile_conversion_utils.sh"; 
    public static final String UNPACK_TEMP_FILE_PATH = "/data/mediadir/inject"; 


    public static Element generateFileElement(String fileUUID, String fileNID, String dirNID, Document mediaStoreFileDocument) throws Exception{

	Element fileEl =  DocumentHelper.createElement("File");
	XMLUtils.addAttribute(fileEl, "id", fileUUID);
	XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
	XMLUtils.addAttribute(fileEl, "fileNID", fileNID);
	if(mediaStoreFileDocument!=null)
		fileEl.add((Element)mediaStoreFileDocument.getRootElement().clone());	
	return fileEl;
    }


    public static void sendPublishDirRequest(Element publishDirInfoEl, DeviceFrameworkI deviceFramework, ResponseHandlerI responseHandler )throws Exception {
        Document publishDirRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/dir/xml/PublishDirRequest.xml");
        Element publishDirRequestDataEl = XMLUtils.getElementXpath(publishDirRequestDoc, "//PublishDirRequestData");
        publishDirRequestDataEl.add((Element)publishDirInfoEl.clone());
        Message publishRequestMessage =  Message.createRequest(publishDirRequestDoc, deviceFramework.getAppserverJID() , deviceFramework.getMyJID());        
        deviceFramework.sendRequest(publishRequestMessage, publishDirRequestDoc, responseHandler);
        
    }
    
    public static void publishDirSizeUpdate(String dirNID, long sizeUsed,DeviceFrameworkI deviceFramework, ResponseHandlerI responseHandler) throws Exception {
		Document publishDirRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/dir/xml/PublishDirSizeRequest.xml");
		Element publishDirSizeEl = XMLUtils.getElementXpath(publishDirRequestDoc, "//PublishDirSize");
		XMLUtils.addAttribute(publishDirSizeEl, "sizeUsed",String.valueOf(sizeUsed));
		XMLUtils.addAttribute(publishDirSizeEl, "byNID",dirNID);
		Message publishRequestMessage =  Message.createRequest(publishDirRequestDoc, deviceFramework.getAppserverJID() , deviceFramework.getMyJID());        
		deviceFramework.sendRequest(publishRequestMessage, publishDirRequestDoc, responseHandler);
    }
    
    public static void publishPurgeFileStatus(String dirNID, long sizeUsed,DeviceFrameworkI deviceFramework, ResponseHandlerI responseHandler) throws Exception {
		Document publishDirRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/dir/xml/PublishDirPurgeFileStatusRequest.xml");
		Element publishDirSizeEl = XMLUtils.getElementXpath(publishDirRequestDoc, "//PublishDirPurgeFileStatus");
		XMLUtils.addAttribute(publishDirSizeEl, "byNID",dirNID);
		XMLUtils.addAttribute(publishDirSizeEl, "state","Ok");
		XMLUtils.addAttribute(publishDirSizeEl, "sizeUsed",String.valueOf(sizeUsed));
		Message publishRequestMessage =  Message.createRequest(publishDirRequestDoc, deviceFramework.getAppserverJID() , deviceFramework.getMyJID());        
		deviceFramework.sendRequest(publishRequestMessage, publishDirRequestDoc, responseHandler);
    }

	public static Element createDownloadElement(String mediaFormat, String downloadState, boolean stale) throws Exception {
		
		Element downloadEl = DocumentHelper.createElement("Download");
		Element mediaEl;
		if (mediaFormat.equals("Both")) {
			mediaEl = createMediaDownloadElement("Proprietary", downloadState, stale);
			downloadEl.add((Element) mediaEl.detach());
			mediaEl = createMediaDownloadElement("Standard", downloadState, stale);
			downloadEl.add((Element) mediaEl.detach());
		} else {
			mediaEl = createMediaDownloadElement(mediaFormat, downloadState, stale);
			downloadEl.add((Element) mediaEl.detach());
		}
		return downloadEl;
	}

	public static void publishPrepareFileStatus(String dirNID, String fileUUID, String mediaFormat, String downloadState, String fileState, Document mediaStoreFileDoc,DeviceI device, ResponseHandlerI responseHandler,String typeOfRequest, String prepareFileName) throws Exception {

		Element downloadEl = createDownloadElement(mediaFormat, downloadState, false);
		sendPublishFileRequestToSC(dirNID, fileUUID,"", false, false, null, mediaStoreFileDoc, downloadEl, device, responseHandler,null,typeOfRequest, prepareFileName);
	}
	
	public static void sendPublishFileRequestToSC(String dirNID, String fileUUID, String queueID, boolean appendSize,boolean isImportRequest, Long size, Document mediaStoreFileDoc, Element downloadEl, DeviceI device, ResponseHandlerI responseHandler,HashMap<String, Element> listOfBookmark,String typeOfRequest, String prepareFileName) throws Exception {
	    String publishFileRequestType = "";
		if (isImportRequest)
			publishFileRequestType = "PublishImportFileRequest";
		else
			publishFileRequestType = "PublishPrepareFileStatusRequest";
		
		sendPublishToSC(dirNID, fileUUID, queueID, appendSize, size, mediaStoreFileDoc, downloadEl, device, responseHandler, listOfBookmark, typeOfRequest, prepareFileName, publishFileRequestType, null);
	}
	
	public static void sendPublishToSC(String dirNID, String fileUUID, String queueID, boolean appendSize, Long size, Document mediaStoreFileDoc, Element downloadEl, DeviceI device, ResponseHandlerI responseHandler,HashMap<String, Element> listOfBookmark,String typeOfRequest, String prepareFileName, String publishFileRequestType, Element deleteBookmarkEl) throws Exception {
	
		Document publishFileRequestDoc;
		publishFileRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/file/xml/"+publishFileRequestType+".xml");
	    Element publishFileRequestDataEl = XMLUtils.getElementXpath(publishFileRequestDoc, "//"+publishFileRequestType+"Data");
	    Element fileEl =  DocumentHelper.createElement("File");
	    XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
	    XMLUtils.addAttribute(fileEl, "id", fileUUID);
	    XMLUtils.addAttribute(fileEl, "queueID", queueID);
	    XMLUtils.addAttribute(fileEl, "appendSize", String.valueOf(appendSize));
	    if(typeOfRequest != null)
	    	XMLUtils.addAttribute(fileEl, "typeOfRequest", typeOfRequest);
	    if(prepareFileName != null)
	    	XMLUtils.addAttribute(fileEl, "prepareFileName", prepareFileName);
	    if(size != null)
	    	XMLUtils.addAttribute(fileEl, "size", String.valueOf(size));
	    if(downloadEl != null)
	    	fileEl.add(downloadEl.detach());
	    
	    fileEl.add(mediaStoreFileDoc.getRootElement().detach());
	    // Bookmarks
	    if (listOfBookmark != null && listOfBookmark.size() > 0) {
		Element bookmarksEl = DocumentHelper.createElement("Bookmarks");
		for (Map.Entry<String, Element> entry : listOfBookmark.entrySet()){
		    Element bookmark = entry.getValue();
		    bookmarksEl.add(bookmark);
		}
		fileEl.add(bookmarksEl);
	    }
	     
	    // Delete Bookmarks
	    if(deleteBookmarkEl != null)
	    	fileEl.add(deleteBookmarkEl.detach());
	  
	    publishFileRequestDataEl.add(fileEl.detach());
	    Message publishRequestMessage =  Message.createRequest(publishFileRequestDoc, device.getAppserverJID() , device.getMyJID());		
	    device.sendRequest(publishRequestMessage, publishFileRequestDoc, responseHandler);

	}
	
	public static void updateBookmarksOfMediaGroup(String dirNID,String dirID, String fileUUID, String queueID, String rootFolderPath, DeviceI device,  String filePath, ResponseHandlerI responseHandler) throws Exception{  
		Scanner scanner=null;
		try{
	     scanner = new Scanner( new File(filePath) );
		String xml = scanner.useDelimiter("\\A").next();
	    Document groupFileDoc = DocumentHelper.parseText(xml);
	    List <?>listOfBookmarks = XMLUtils.getListOfNodesXpath(groupFileDoc, "//Bookmark");
	    for (Object bookmarkObj : listOfBookmarks) {
		Element bookmarkEL = (Element) bookmarkObj;		 
		device.getMediaStoreAgentImpl().addBookmark(dirID, dirNID, fileUUID, UUID.randomUUID().toString(), "", XMLUtils.elementToDocument(bookmarkEL));
	    }
	    MediaStoreAgentI mediaStoreAgent = device.getMediaStoreAgentImpl();
	    Document mediaStoreFileDoc = mediaStoreAgent.getMediaStoreFileDocument(dirID, dirNID, fileUUID);
	    //XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, "Ready");	   	      
	    mediaStoreAgent.setFileRequest("", dirNID, "" , fileUUID, mediaStoreFileDoc);	   
	    sendPublishFileRequestToSC(dirNID, fileUUID, queueID, false, true, null, mediaStoreFileDoc, null, device, responseHandler, mediaStoreAgent.getMediaStoreBookMarks(dirID, dirNID, fileUUID), null, null);
	    cleanupImportTempFiles(filePath,null);
	  }finally {
		  if(scanner != null)
			  scanner.close();
	  }
	    
	}
	public static Element createMediaDownloadElement(String mediaFormat, String downloadState, boolean stale) {
		Element mediaEl = DocumentHelper.createElement("Media");
		mediaEl.addAttribute("format", mediaFormat);
		mediaEl.addAttribute("state", downloadState);
		mediaEl.addAttribute("stale", String.valueOf(stale));
		return mediaEl;
	}

	public static void cleanupImportTempFiles(String tarPath, String fileUUID) throws Exception{ 
	    File tarPath_file = new File(tarPath); 
	    tarPath_file.delete(); 
	    if(fileUUID != null){
		File temp_folder = new File(UNPACK_TEMP_FILE_PATH, fileUUID); 
		if (temp_folder.exists()) { 
		    String[] cmdUntar = { "rm", "-rf", temp_folder.getAbsolutePath() }; 
		    StringBuffer buf = new StringBuffer(); 
		    int retVal = Utils.executeProcess(cmdUntar, buf); 
		    if (retVal != 0) { 
			String error = "Error during cleanupImportTempFiles :=" + UNPACK_TEMP_FILE_PATH + "  retVal=" + retVal + " " + MediaStoreUtils.joinArray(cmdUntar, " ") + " " + buf.toString();	                 
			throw new Exception(error); 
		    } 
		} 
	    }
	}
	public static String joinArray(String[] array, String saparator) { 
	    if (array != null) { 
		StringBuffer buf = new StringBuffer(); 
		for (int i = 0; i < array.length; i++) { 
		    if (i != 0) { 
			buf.append(saparator); 
		    } 
		    buf.append(array[i]); 
		} 
		return buf.toString(); 
	    } 
	    return saparator; 
	} 

	
	public static Document publishOffloadFileStatus(String state, String error, String fileNID, int uploadState, String dirNID) throws Exception {
		Document publishOffloadFileRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/sessionservice/resource/xml/OffloadStateRequest.xml");
		Element headerEl = XMLUtils.getElementXpath(publishOffloadFileRequestDoc, "//Header");
		XMLUtils.addAttribute(headerEl, "state", uploadState+"");
		Element requestDataEl = XMLUtils.getElementXpath(publishOffloadFileRequestDoc, "//OffloadStateRequestData");
		Element offloadFileEl= DocumentHelper.createElement("OffloadFile");
		XMLUtils.addAttribute(offloadFileEl, "dirNID", dirNID);
		XMLUtils.addAttribute(offloadFileEl, "NID", fileNID);
		XMLUtils.addAttribute(offloadFileEl, "errordesc", error);
		XMLUtils.addAttribute(offloadFileEl, "error", state);
		requestDataEl.add(offloadFileEl);
		return publishOffloadFileRequestDoc;
	}
	public static boolean isMediaGroupFilePresent(String mediaGroupID, String path, String uuid) throws Exception{
	
	    String [] cmdsh = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, "is_mediagroup_present", path, mediaGroupID,uuid,"&" };
	    System.out.println("mediafile_unpack :=" + MediaStoreUtils.joinArray(cmdsh, " "));
	    StringBuffer buf = new StringBuffer();
	    int retVal = Utils.executeProcess(cmdsh, buf);
	    if (retVal != 0) {		
		System.out.println("No Group file present for the file -'"+ mediaGroupID + "'");
		return false;
	    }
	    else{
		return true;
	    }
	}

}
