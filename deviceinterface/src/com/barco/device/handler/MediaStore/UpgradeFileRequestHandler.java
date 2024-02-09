package com.barco.device.handler.MediaStore;

import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;

public class UpgradeFileRequestHandler implements MessageHandlerI {
	
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document requestDoc = message.getMessageDoc();

		Element requestElement=XMLUtils.getElementXpath(requestDoc, "//UpgradeFile");
		String dirNID = XMLUtils.getAttributeValue(requestElement, "byDirNID"); 
		String dirID = XMLUtils.getAttributeValue(requestElement, "byDirID");
		List<?> fileList = XMLUtils.getListOfNodesXpath(requestDoc, "//File");
		Element resultEl = DocumentHelper.createElement("FileList");	
		Set<String> fileNIDs = new HashSet<String>();
		
		for(Object object:fileList) {
			Element fileEl = (Element) object;
			String fileUUID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.BY_ID_ATTRIBUTE_NAME, "");
			String fileNID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.NID_ATTRIBUTE_NAME, "");
			Document mediaStoreFileDocument = null;
			if(fileNIDs.add(fileNID) && !device.getMediaStoreAgentImpl().isMediaUpgraded(dirID, dirNID, fileUUID))
			{
				Element upgradedFileEl = null;
				Element upgradedGroupFileEl = null;
				Element groupbookmarkUUIDListEl = null;
				Element bookmarkUUIDListEl = null;

				try {			
					mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument(dirID, dirNID, fileUUID);
					String title = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.TITLE_ATTRIBUTE_NAME, "");
					String type = XMLUtils.getAttributeValueWithDefault(fileEl, "type", "");
					Element metadataEl = XMLUtils.getElementXpathFromElement(fileEl, ".//"+  MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
					Element mediaStreamProfileInfoEl = XMLUtils.getElementXpathFromElement(fileEl, ".//MediaStreamProfileInfo");
					Element bookmarkListEl =  XMLUtils.getElementXpathFromElement(fileEl, "./BookmarkList[1]");

					List<?> bookmarkList = XMLUtils.getListOfNodesXpath(XMLUtils.elementToDocument(bookmarkListEl), "//Bookmark");
					bookmarkUUIDListEl =createOrUpdateBookmarks(bookmarkList,dirID,dirNID,fileUUID,"",device,fileNID);
					if(bookmarkList.size()!=XMLUtils.getListOfNodesXpath(XMLUtils.elementToDocument(bookmarkUUIDListEl), "//bookmark").size())
						throw new ServiceException(MediaStoreServiceExceptionCodes.CREATION_OF_BOOKMARK_FOLDER_FAILED, "Error while creating bookmark");
					// Support Groups
					Element groupElement = XMLUtils.getElementXpathFromElement(fileEl, ".//GroupInfo");
					String groupFileUUID = XMLUtils.getAttributeValueWithDefault(groupElement,"UUID","");
					if(!groupFileUUID.isEmpty()){  //check to identify synch clip
						String groupFileNID = XMLUtils.getAttributeValueWithDefault(groupElement,"NID","");
						device.getLogger().info("UpgradeFileRequestHandler.handleMessage groupFileNID +"+ groupFileNID + "groupFileUUID" +groupFileUUID);
						String groupFileState = XMLUtils.getAttributeValueWithDefault(groupElement,"state","");

						Document groupMediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument(dirID, dirNID, groupFileUUID);

						if(groupMediaStoreFileDocument == null){
							//create new mediagroup store file
							device.getLogger().info("create new mediagroup store file : "+ groupFileUUID );
							groupElement = (Element)groupElement.clone();
							groupElement.setName("File");
							groupElement.addAttribute("type", MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString());
							groupElement.addAttribute("streamType", "");
							groupElement.addAttribute(MediaStoreServiceConst.MEDIA_GROUP_ID_ATTRIBUTE_NAME, XMLUtils.getValueXpath(mediaStoreFileDocument, ".//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, ""));
							groupElement.addAttribute(MediaStoreServiceConst.CLIP_ID_ATTRIBUTE_NAME, XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpath(mediaStoreFileDocument,".//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME), MediaStoreServiceConst.CLIP_ID,""));
							new AddFileRequestHandler().initRecording(groupElement, device, dirID, dirNID, groupFileState, groupFileUUID, groupFileNID);
							String upgradedGroupFile = "<File id='"+groupFileUUID+"' dirNID='"+dirNID+"' fileNID='"+groupFileNID+"'></File>";
							upgradedGroupFileEl = XMLUtils.stringToElement(upgradedGroupFile);
						}else{
							device.getLogger().info("update mediagroup store file : "+ groupFileUUID );
							String groupFileTitle = XMLUtils.getAttributeValueWithDefault(groupElement,"title","");
							Element groupFileMetadataEl = XMLUtils.getElementXpathFromElement(groupElement,".//"+  MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
							updateDataOfMediaStoreDoc(groupFileTitle,"MediaGroup",groupFileMetadataEl,null,groupMediaStoreFileDocument );
							upgradedGroupFileEl = device.getMediaStoreAgentImpl().upgradeFile(dirID, dirNID, groupFileNID, groupFileUUID, groupMediaStoreFileDocument);
						}
						//mediagroup bookmark support
						Element groupBookmarkListEl = XMLUtils.getElementXpathFromElement(groupElement, ".//BookmarkList"); 
						List<?> groupBookmarkList = XMLUtils.getListOfNodesXpath(XMLUtils.elementToDocument(groupBookmarkListEl), "//Bookmark");
						groupbookmarkUUIDListEl = createOrUpdateBookmarks(groupBookmarkList,dirID,dirNID,groupFileUUID,fileUUID,device, fileNID);
						if(groupBookmarkList.size()!=XMLUtils.getListOfNodesXpath(XMLUtils.elementToDocument(groupbookmarkUUIDListEl), "//bookmark").size())
							throw new ServiceException(MediaStoreServiceExceptionCodes.CREATION_OF_BOOKMARK_FOLDER_FAILED, "Error while creating bookmark");
						upgradedGroupFileEl.add(groupbookmarkUUIDListEl);
						resultEl.add(upgradedGroupFileEl);
					}
					//update the file mediaStoreFileDocument 
					updateDataOfMediaStoreDoc(title,type,metadataEl,mediaStreamProfileInfoEl,mediaStoreFileDocument );
					upgradedFileEl = device.getMediaStoreAgentImpl().upgradeFile(dirID, dirNID, fileNID, fileUUID, mediaStoreFileDocument);
					upgradedFileEl.add(bookmarkUUIDListEl);
					resultEl.add(upgradedFileEl);
				} catch (Exception e) {
					//if we are only upgrading one file and we have some issues with upgrade then throw exception.  Otherwise skip that file and continue with rest of the list
					if(fileList.size() == 1) {
						throw e;
					}				
					device.getLogger().error("Can not upgrade File. FileUUID:="+fileUUID +" fileNID:="+fileNID +" errorMessage:="+e.getMessage());
					if(upgradedFileEl!= null){			
						if(bookmarkUUIDListEl!=null && XMLUtils.getElementXpathFromElement(upgradedFileEl, ".//BookmarkUUIDList")!=null){
							upgradedFileEl.add(bookmarkUUIDListEl);

						}
					} else {
						upgradedFileEl = MediaStoreUtils.generateFileElement(fileUUID, fileNID, dirNID, mediaStoreFileDocument);
					}
					XMLUtils.addAttribute(upgradedFileEl, "upgradeState", "Error");
					resultEl.add(upgradedFileEl);			
				}
			} else {
				mediaStoreFileDocument = device.getMediaStoreAgentImpl().getMediaStoreFileDocument(dirID, dirNID, fileUUID);
				resultEl.add(MediaStoreUtils.generateFileElement(fileUUID, fileNID, dirNID, mediaStoreFileDocument));
				device.getLogger().info("File already upgraded : fileUUID:=" +fileUUID +" fileNID :="+fileNID);
				 
			}
		}
		message.setResult(XMLUtils.elementToDocument(resultEl));

	}

	void updateDataOfMediaStoreDoc(String title,String type, Element metaDataEl, Element mediaStreamProfileInfoEl, Document mediaStoreFileDocument ){
		//we can copy more info if required later
		try{
			if(!title.isEmpty()) {
				XMLUtils.setValueXpath(mediaStoreFileDocument, "//"  +MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
			}

			if(!type.isEmpty()) {
				XMLUtils.setValueXpath(mediaStoreFileDocument, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, type);
			}
			if(metaDataEl != null) {
				XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDocument, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEl);
			}
			if(mediaStreamProfileInfoEl != null) {
				XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDocument, "//MediaStreamProfileInfo"), mediaStreamProfileInfoEl);
			}
		}catch(Exception e){
			e.printStackTrace();}

	}
	//prepare new or updated bookmarkFileDoc and return
	Document getBookMarkdoc(Element bookmarkElement, Document bookmarkFileDoc ) throws Exception{

		String tc = XMLUtils.getValueXpath(bookmarkElement, ".//"+ MediaStoreServiceConst.TC,"0");
		String offset = XMLUtils.getValueXpath(bookmarkElement, ".//"+MediaStoreServiceConst.OFFSET ,"0");
		String bookmarkTitle = XMLUtils.getValueXpath(bookmarkElement, ".//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,"");
		String comment = XMLUtils.getValueXpath(bookmarkElement, ".//"+MediaStoreServiceConst.COMMENT,"");
		Element metaDataEL = XMLUtils.getElementXpathFromElement(bookmarkElement, ".//MetaData");   
		String currentTime = String.valueOf(System.currentTimeMillis());
		if(bookmarkFileDoc ==null){
			bookmarkFileDoc = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/BookmarkStoreFile.xml");
			XMLUtils.setValueXpath(bookmarkFileDoc, "//"+MediaStoreServiceConst.DATE_CREATED,currentTime);
			XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.TC,tc);
			XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.OFFSET,offset);
		}
		if(!bookmarkTitle.isEmpty()) {
			XMLUtils.setValueXpath(bookmarkFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, bookmarkTitle);
		}

		if(!comment.isEmpty()) {
			XMLUtils.setValueXpath(bookmarkFileDoc, "//"+  MediaStoreServiceConst.COMMENT, comment);
		}

		if(metaDataEL != null) {
			XMLUtils.replaceContent(XMLUtils.getElementXpath(bookmarkFileDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEL);
		}

		XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,currentTime);    

		return bookmarkFileDoc;
	}

	Element createOrUpdateBookmarks(List<?> bookmarkList,String dirID,String dirNID,String fileUUID, String synchClipId, DeviceI device, String fileNID)throws Exception{
		Element bookmarkUUIDListEl = DocumentHelper.createElement("BookmarkUUIDList");
		if(bookmarkList!=null && !bookmarkList.isEmpty()){ //mediafile has bookmarks
			device.getLogger().info("handleBookmarks: "+ "dirID "+dirID + " dirNID "+ dirNID + " fileUUID "+fileUUID + " fileNID "+fileNID+" synchClipId "+synchClipId);
			for(Object bm:bookmarkList) {
				Element bookmarkElement = (Element) bm;
				
				String bookmarkNID = XMLUtils.getAttributeValue(bookmarkElement, MediaStoreServiceConst.NID);
				String bookmarkUUID = XMLUtils.getValueXpath(bookmarkElement, ".//"+MediaStoreServiceConst.UUID, "") ;
				try {
					boolean ismediaStoreBookmarkDocumentExist = device.getMediaStoreAgentImpl().isMediaStoreBookmarkExist(dirID,dirNID, fileUUID, bookmarkUUID);
					Document bookmarkDoc = null;			
					if(!ismediaStoreBookmarkDocumentExist){
						bookmarkUUID = bookmarkNID;//UUID.randomUUID().toString(); 
						//Create new bookmark for file
						bookmarkDoc = getBookMarkdoc(bookmarkElement,null);
						device.getLogger().info("Start create new bookmark for file : "+ fileUUID+" : bookmarUUID: "+bookmarkUUID);
						device.getMediaStoreAgentImpl().addBookmark(dirID, dirNID, fileUUID,bookmarkUUID,synchClipId,bookmarkDoc);
						device.getLogger().info("End creating new bookmark for file : "+ fileUUID+" : bookmarUUID: "+bookmarkUUID);
						String bookmarkData = "<bookmark NID='"+bookmarkNID+"' UUID='"+bookmarkUUID+"'></bookmark>";
						Element bookmarkDataEl=XMLUtils.stringToElement(bookmarkData);
						bookmarkUUIDListEl.add(bookmarkDataEl);
					}else{
						//update the existing bookmark
						Document mediaStoreBookmarkDocument = device.getMediaStoreAgentImpl().getMediaStoreBookmarkDocument(dirID, dirNID, fileUUID, bookmarkUUID);
						bookmarkDoc = getBookMarkdoc(bookmarkElement,mediaStoreBookmarkDocument);
						device.getLogger().info("update bookmark for file : "+ fileUUID );
						String timecode = XMLUtils.getValueXpath(bookmarkDoc,"//"+MediaStoreServiceConst.TC,"0");  
						String offSet  = XMLUtils.getValueXpath(bookmarkDoc,"//"+MediaStoreServiceConst.OFFSET,"0");
						device.getMediaStoreAgentImpl().updateBookmark(dirID, dirNID, fileUUID, bookmarkUUID, bookmarkDoc, timecode, offSet,synchClipId); 
					}
				} catch (Exception e) {
					e.printStackTrace();
					device.getLogger().error("Can not Add/Update bookmark while upgrading file :"+fileUUID + " bookmarkNID:="+bookmarkNID+ " bookmarkUUID:="+bookmarkUUID+ "   "+e.getMessage());
				} 
			}
		}
		return  bookmarkUUIDListEl;

	}	
}
