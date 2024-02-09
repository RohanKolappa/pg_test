package com.barco.device.handler.MediaStore;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.List;
import java.util.StringTokenizer;
import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;



public class AddFileRequestHandler implements MessageHandlerI,ResponseHandlerI {

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {	
		Element requestElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//AddFile");
		String dirId = XMLUtils.getAttributeValueWithDefault(requestElement, "byDirID","");
		String dirNID = XMLUtils.getAttributeValueWithDefault(requestElement, "byDirNID","");
		String imageType = XMLUtils.getAttributeValueWithDefault(requestElement, "imageType", "");
		String queueID = XMLUtils.getAttributeValueWithDefault(requestElement, "queueID", "");		
		String fileUUID = XMLUtils.getAttributeValueWithDefault(requestElement, "uuid", "");
		String filePath = XMLUtils.getAttributeValueWithDefault(requestElement, "filePath", "");
		String hlsenable = XMLUtils.getAttributeValueWithDefault(requestElement, "hlsenable", "false");
		boolean ingestMedia = Boolean.valueOf(XMLUtils.getAttributeValueWithDefault(requestElement, "ingestMedia","false"));
		boolean publishToSC = false;
		Document resultDoc = null;

		if(fileUUID.isEmpty())
			fileUUID = UUID.randomUUID().toString();
		
		//handle snapshot and mp4 ingest
		if(ingestMedia) {
			if(imageType.equals("MP4")) {
				requestElement.addAttribute("streamType","RTP");
			}
			Document mediaStoreFileDocument  = generateMediaStoreFile(requestElement, device, dirId, dirNID,MediaStoreServiceConst.NOT_READY,fileUUID,"", imageType);
			resultDoc = device.getMediaStoreAgentImpl().ingestMedia(filePath, String.valueOf(System.currentTimeMillis()), dirId, dirNID, fileUUID, mediaStoreFileDocument, hlsenable);
			publishToSC = true;	
			message.setAsynch(true);

		} else {

			resultDoc = createMediaStoreFile(requestElement, device, dirId, dirNID,MediaStoreServiceConst.NOT_READY,fileUUID,"", imageType);

			String mediaGroupID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.MEDIA_GROUP_ID_ATTRIBUTE_NAME,"");	
			if("MKD".equals(imageType) || "GRP".equals(imageType)){
				String mediaGroupTitle = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.MEDIA_GROUP_TITLE_ATTRIBUTE_NAME,"");
				String mediaGroupNID = XMLUtils.getAttributeValueWithDefault(requestElement, MediaStoreServiceConst.MEDIA_GROUP_NID_ATTRIBUTE_NAME,"");
				String isOldMediaWithGroup = XMLUtils.getAttributeValueWithDefault(requestElement, "isOldMediaWithGroup",""); 
				if(!filePath.isEmpty()) { 
					dirNID = resultDoc.getRootElement().attributeValue("dirNID"); 
					String dateModified = XMLUtils.getValueXpath(resultDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,"");

					if("MKD".equals(imageType)){
						publishFileToSC(resultDoc.getRootElement(), message, device); // only publish if mkd, possible race condition for grp files
						try{ 
							device.getMediaStoreAgentImpl().unpackImportFile(dirId, dirNID, fileUUID,dateModified, filePath, queueID, mediaGroupID,mediaGroupNID,isOldMediaWithGroup, mediaGroupTitle);  
						}  		     
						catch(Exception e){ 
							e.printStackTrace(); 
						}
					}
					else{			     
						String rootPath= device.getMediaStoreAgentImpl().getDirPath(dirNID, dirId);		
						MediaStoreUtils.updateBookmarksOfMediaGroup(dirNID,dirId, fileUUID, queueID, rootPath,device, filePath, this);
					}
				}	 
				message.setAsynch(true);
			}
		}	

		if(publishToSC) {
		    publishFileToSC(resultDoc.getRootElement(), message, device);
		}		
		message.setResult(resultDoc);
	}




	public Document createMediaStoreFile(Element fileElement,DeviceI device,String dirId,String dirNID,String state,String fileUUID,String fileNID, String imageType) throws Exception {

		Document mediaStoreFileDoc = generateMediaStoreFile(fileElement, device, dirId, dirNID, state, fileUUID, fileNID, imageType);		

		return  device.getMediaStoreAgentImpl().addFile(dirId, dirNID, fileUUID, mediaStoreFileDoc);

	}



	public Document generateMediaStoreFile(Element fileElement,DeviceI device,String dirId,String dirNID,String state,String fileUUID,String fileNID, String imageType) throws Exception {

		String title  =  XMLUtils.getAttributeValueWithDefault(fileElement, "title", "");
		String type = XMLUtils.getAttributeValueWithDefault(fileElement, "type", "");
		String streamType = XMLUtils.getAttributeValueWithDefault(fileElement, "streamType", "");
		String description = XMLUtils.getValueXpath(fileElement, "./Description", "");
		Element metaDataEL = XMLUtils.getElementXpathFromElement(fileElement, ".//MetaData");
		Element profileXML= XMLUtils.getElementXpathFromElement(fileElement, ".//ProfileXML");

		Document mediaStoreFileDoc = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/MediaStoreFile.xml");	
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, type);
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, description);
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, streamType);
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
		String currentTime = String.valueOf(System.currentTimeMillis());
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.DATE_CREATED,currentTime);
		XMLUtils.setValueXpath(mediaStoreFileDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,currentTime);
		XMLUtils.addAttribute(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, device.getServiceVersion());
		if(!imageType.isEmpty()) {
			XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.IMAGE_TYPE_ELEMENT, imageType);
		}

		//update media group information 
		Element mediaGroupELement= XMLUtils.getElementXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);		
		String mediaGroupID = XMLUtils.getAttributeValueWithDefault(fileElement, MediaStoreServiceConst.MEDIA_GROUP_ID_ATTRIBUTE_NAME,"");		
		String clipID = XMLUtils.getAttributeValueWithDefault(fileElement, MediaStoreServiceConst.CLIP_ID_ATTRIBUTE_NAME, "");		
		mediaGroupELement.setText(mediaGroupID);
		XMLUtils.addAttribute(mediaGroupELement, MediaStoreServiceConst.CLIP_ID, clipID);


		for( MediaStoreServiceConst.MEDIA_TYPE_ENUMS typeEnum : MediaStoreServiceConst.MEDIA_TYPE_ENUMS.values()) {
			if(!typeEnum.toString().equals(type)) {
				XMLUtils.getElementXpath(mediaStoreFileDoc, "//" + typeEnum.toString()).detach();
			}
		}
		
		if(!streamType.isEmpty() && profileXML == null){
			profileXML  = DocumentHelper.createElement("MediaStreamProfileInfo");
			Element profileXMLEL = XMLUtils.getElementXpath(mediaStoreFileDoc, "//"+streamType+"MediaStreamProfileInfo");
			profileXML.add((Element)profileXMLEL.clone());
		}
   
		if(profileXML!=null) {
			XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDoc, "//MediaStreamProfileInfo"), profileXML);           
		}

		//update metadata element
		if(metaDataEL!=null) {
			XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEL);
		}


		return  mediaStoreFileDoc;

	}


	public void initRecording(Element fileElement,DeviceI device, String dirId,String dirNID,String state,String fileUUID,String fileNID) throws Exception {

		Document mediaStoreFileDoc = device.getMediaStoreAgentImpl().getMediaStoreFileDocument(dirId, dirNID, fileUUID);
		Element fileEL = null;
		if(mediaStoreFileDoc == null) {
			Document fileDoc = createMediaStoreFile(fileElement, device, dirId, dirNID, state, fileUUID, fileNID, "");
			fileEL = fileDoc.getRootElement();
		} else {
			Element metaDataEL = XMLUtils.getElementXpathFromElement(fileElement, ".//MetaData");
			Element profileXML= XMLUtils.getElementXpathFromElement(fileElement, ".//ProfileXML");
			XMLUtils.setValueXpath(mediaStoreFileDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,String.valueOf(System.currentTimeMillis()));

			if(profileXML!=null) {
				XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDoc, "//MediaStreamProfileInfo"), profileXML);
			}
			//update metadata element
			if(metaDataEL!=null) {
				XMLUtils.replaceContent(XMLUtils.getElementXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME), metaDataEL);
			}
			
			XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
			
			mediaStoreFileDoc = device.getMediaStoreAgentImpl().setFileRequest(dirId, dirNID, fileNID, fileUUID, mediaStoreFileDoc);
			
			fileEL = MediaStoreUtils.generateFileElement(fileUUID, fileNID, dirNID, mediaStoreFileDoc);
		}

		publishFileToSC(fileEL, null, device);
	}
	

	public void publishFileToSC(Element fileEL, Message message, DeviceI device) throws Exception {
		Document publishFileRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/file/xml/PublishFileRequest.xml");
		Element publishFileRequestDataEl = XMLUtils.getElementXpath(publishFileRequestDoc, "//PublishFileRequestData");
		XMLUtils.addAttribute(fileEL, "eventClockTime", String.valueOf(System.currentTimeMillis()));
		publishFileRequestDataEl.add((Element)fileEL.clone());
		
		if(message != null) {
			String clientData = message.getClientData();
			if(!clientData.isEmpty()) {
				String clientDataString = clientData + MediaStoreServiceConst.SEPARATOR + message.getMessageBusClassName();
				Element requestEl = (Element) message.getMessageDoc().getRootElement().clone();
				Element clientDataEl = XMLUtils.getElementXpathFromElement(requestEl, ".//ClientData");
				clientDataEl.clearContent();
				clientDataEl.setText(clientDataString);
				clientDataString = URLEncoder.encode(requestEl.asXML(), "UTF-8");
				XMLUtils.setValueXpath(publishFileRequestDoc, "//ClientData", clientDataString);
			}
			String protocol = XMLUtils.getValueXpath(message.getMessageDoc()
					.getDocument(), "//Header/Protocol", "http");
			XMLUtils.addElementXpath(publishFileRequestDoc, "//Header",
					"Protocol", protocol);
		}
		Message publishRequestMessage =  Message.createRequest(publishFileRequestDoc, device.getAppserverJID() , device.getMyJID());
		device.sendRequest(publishRequestMessage, publishFileRequestDoc, this);
	}

	@Override
	public void handleResponse(Message response, Document context,	DeviceI device) throws Exception {
		device.getLogger().error("AddFileRequest: Publish File Response Passed");
		
		Document responseDoc = response.getMessageDoc();	
		String clientDataString = XMLUtils.getValueXpath(responseDoc, "//ClientData", "");
		Document requestDoc = null;
	
		if(!clientDataString.isEmpty()) {
			try {
				requestDoc = XMLUtils.stringToDocument(URLDecoder.decode(clientDataString, "UTF-8"));
			} catch (Exception e) {
				return;
			}
			clientDataString = XMLUtils.getValueXpath(requestDoc, "//ClientData", "");
			StringTokenizer st = new StringTokenizer(clientDataString, MediaStoreServiceConst.SEPARATOR);
			String messageBusType = "";
			String clientData = "";
			if (st!= null && st.hasMoreTokens()) {
				clientData = st.nextToken();
				messageBusType = st.nextToken();
			}
			if(clientData.isEmpty() || messageBusType.isEmpty()) {
				return;
			}
			response.setMessageBusClassName(messageBusType);
			response.setClientData(clientData);

			Boolean ingestMedia = Boolean.valueOf(XMLUtils.getValueXpath(requestDoc, "//AddFile/@ingestMedia", "false"));
	
			List<?> fileList = XMLUtils.getListOfNodesXpath(responseDoc, "//File");
			if(fileList != null && !fileList.isEmpty()) {

				for(Object obj:fileList) {
					Element fileEl = (Element)obj;
					if(ingestMedia) {
						// MP4 ingestion doesn't contain mediaURL
						String mediaURL = XMLUtils.getAttributeValueWithDefault(fileEl, "mediaURL", "");
						XMLUtils.addAttribute(fileEl, "url", mediaURL);
					}
				}
			}
			
			device.sendMessage(response);
		}
		
		
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device)	throws Exception {
		device.getLogger().error("AddFileRequest: Publish File Response Failed" +response.getMessageDoc().asXML());
	}

	/*private boolean isGroupFileCreate(String mediaGroupID){

	}*/

}
