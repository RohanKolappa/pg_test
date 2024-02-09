package com.barco.device.handler.Layout;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.StreamEntry;
import com.barco.device.handler.MediaStore.MediaStoreEventHandler;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.utils.XMLUtils;

public class SSEventHandler implements MessageHandlerI{

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Element ssEvent = XMLUtils.getElementXpath(message.getMessageDoc(), "//SSEvent");
		
		String state = XMLUtils.getAttributeValueWithDefault(ssEvent, "state","");
		if(state.equals("Stopped")){
			handleStreamStopped(device, ssEvent);
		} else if(state.equals("Connected" )){
			device.getMediaAgentImpl().handleConnected(message.getMessageDoc());			
		}
		
		
	}
	
	public void handleStreamStopped(DeviceI device,Element eventElement) {
		
		String fileUUID = XMLUtils.getAttributeValueWithDefault(eventElement, "fileUUID", "");
		
		try {
			
			device.getMediaAgentImpl().handleStopped(eventElement.getDocument());
			StreamEntry streamEntry = device.getDeviceDB().getStreamCache().getStreamEntryUsingFileUUID(fileUUID);
			//if no stream found update playlength and size in media store 
			if(streamEntry == null) {
				device.getLogger().info("SSEventHandler: No Stream Found for file:"+fileUUID+" Update playlength: and Size in local store file and publish update to SC");
				
				//update file 
				eventElement.addAttribute("state", MediaStoreServiceConst.READY);
				new MediaStoreEventHandler().handleFileEvent(eventElement, device);
				
				//update disk size
				eventElement.addAttribute("publishUpdate","true");
				new MediaStoreEventHandler().handleDiskSpaceUpdateEvent(eventElement, device);				
				
			} else {
				device.getLogger().info("SSEventHandler: Found stream for file:"+fileUUID+" Update playlength: and Size in cache");
				//update in stream entry cache playlength and size
				Document streamDataDoc = streamEntry.streamDataDoc;
				String playLength = XMLUtils.getAttributeValueWithDefault(eventElement, "playLength", "");
				String size = XMLUtils.getAttributeValueWithDefault(eventElement, "size", "");
				
				Element destFilePlaylength = DocumentHelper.createElement("destFilePlaylength");
				destFilePlaylength.setText(playLength);
				streamDataDoc.getRootElement().add(destFilePlaylength);
				
				Element destFileSize = DocumentHelper.createElement("destFileSize");
				destFileSize.setText(size);
				streamDataDoc.getRootElement().add(destFileSize);

			}			
			
		} catch (Exception e) {
			e.printStackTrace();
			device.getLogger().error("SSEventHandler: Error while handling ssevent " + fileUUID +" \n"+e.toString());			
		}
	}

}
