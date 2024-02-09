package com.barco.device.handler.MediaStore;

import java.util.List;

import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class DeleteFileRequestHandler
implements MessageHandlerI
{
	public void handleMessage(Message message, DeviceI device)
			throws Exception
			{

		Element deleteFileRequestEL = XMLUtils.getElementXpath(message.getMessageDoc(), "//" + MediaStoreServiceConst.DELETE_FILE_REQUEST_ELEMENT);
		String dirNID = XMLUtils.getAttributeValueWithDefault(deleteFileRequestEL, MediaStoreServiceConst.BY_DIR_NID_ATTRIBUTE_NAME, "");
		String dirID = XMLUtils.getAttributeValueWithDefault(deleteFileRequestEL, MediaStoreServiceConst.BY_DIR_ID_ATTRIBUTE_NAME, "");
		boolean purgeFiles = Utils.getBooleanValue(XMLUtils.getAttributeValueWithDefault(deleteFileRequestEL, "purgeFiles","false"), false);
		List<?> fileList = XMLUtils.getListOfNodesXpath(message.getMessageDoc(), "//" + MediaStoreServiceConst.FILE_ELEMENT);
		Element resultEl = DocumentHelper.createElement("FileList");

	
		
		//if purge files we have to update disk size in the database.TBD: Test this code when implementing ILM
		if(purgeFiles) {
			device.getMediaStoreAgentImpl().startPurgeFileProcess(dirID, dirNID, fileList);	
			//create result element
			for(Object obj: fileList) {
				Element fileEl = (Element)obj;
				String fileNID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.FILE_NID_ATTRIBUTE_NAME, "");
				Element fileElememt = DocumentHelper.createElement("File");
				XMLUtils.addAttribute(fileElememt, MediaStoreServiceConst.NID_ATTRIBUTE_NAME, fileNID);			
				resultEl.add(fileElememt);
			}
			
		} else {
			for(Object obj: fileList) {
				Element fileEl = (Element)obj;
				String fileUUID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.BY_ID_ATTRIBUTE_NAME, "");
				String fileNID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.FILE_NID_ATTRIBUTE_NAME, "");

				device.getMediaStoreAgentImpl().deleteFile(dirID, dirNID, fileNID, fileUUID, purgeFiles);

				Element fileElememt = DocumentHelper.createElement("File");
				XMLUtils.addAttribute(fileElememt, MediaStoreServiceConst.NID_ATTRIBUTE_NAME, fileNID);			
				resultEl.add(fileElememt);
			}
		}
		
     	message.setResult(XMLUtils.elementToDocument(resultEl));

	}


}