package com.barco.device.handler.Media;



import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.utils.MediaRoomServiceExceptionCodes;
import com.barco.utils.MediaUtils;
import com.barco.utils.XMLUtils;

public class StartRequestHandler implements MessageHandlerI{
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        String requestNID = XMLUtils.getValueXpath(doc, "//requestNID", "");
        String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");
        
        Document streamDataDoc = MediaUtils.parseDataDoc(XMLUtils.getElementXpath(doc, "//streamdatadoc"));
        Document mediaSourceDataDoc = MediaUtils.parseDataDoc(XMLUtils.getElementXpath(doc, "//mediasourcedatadoc"));
        MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
        
        String roomNID = XMLUtils.getValueXpath(streamDataDoc, "//roomNID", "");
        String relayNID = XMLUtils.getValueXpath(streamDataDoc, "//relayNID", "");
        if(!mediaAgentImpl.getRoomNIDList().contains(roomNID)) {
        	throw new ServiceException(MediaRoomServiceExceptionCodes.ROOMIDNOTFOUND, "StartRequestHandler:handleMessage not found roomNID=" + roomNID);
        }
        
        String sourceType = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
        String lookbackSourceAgentJID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceAgentJID","");
        if((sourceType.equals("MEDIAFILE") && "".equals(relayNID))) {
        	updateSourceFileParentDirPath(mediaSourceDataDoc, device);
        }
        if(sourceType.equals("ENCODER") && lookbackSourceAgentJID.equals(device.getMyJID())) {
        	updateSourceFileParentDirPath(mediaSourceDataDoc, device);
        }
        Document streamStatusDoc = mediaAgentImpl.startStream(device, requestNID, streamNID, streamDataDoc, mediaSourceDataDoc);        
        message.setResult(streamStatusDoc);
        
        // Testing exception handling
        // throw new ServiceException(ServiceExceptionCodes.HANDLERERROR, "StartStreamRequestHandler:handleMessage error");
	}
	
	public void updateSourceFileParentDirPath(Document mediaSourceDataDoc,
			DeviceI device) throws Exception {
		String dirNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirNID", "");
		if (!dirNID.isEmpty()) {
			String dirRootPath = device.getMediaStoreAgentImpl().getDirPath(dirNID, "");
			Element srcFileParentDirPath = DocumentHelper.createElement("lookbackSourceFileParentDirPath");
			srcFileParentDirPath.setText(dirRootPath);
			mediaSourceDataDoc.getRootElement().add(srcFileParentDirPath);
		}
	}
	
}
