package com.barco.devicevng.MediaStore.responsehandlers;

import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class PublishFileResponseHandler implements ResponseHandlerI{

	@Override
	public void handleResponse(Message response, Document context,
			DeviceI device) throws Exception {
		device.getLogger().info("Calling start sync from response handler");
		sendStartSynchRequest(response, device);
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device)
			throws Exception {
		device.getLogger().info("ERROR from SC: Calling start sync from error handler");
		sendStartSynchRequest(response, device);

	}
	
	private void sendStartSynchRequest(Message response, DeviceI device) throws Exception{
		String clientData = URLDecoder.decode(response.getClientData(),"UTF-8");
		device.getLogger().info("Client data at PublishFileResponseHandler :"+clientData);
		if(clientData!=null && !clientData.isEmpty()){
			try{
				Element callBackInfo = null;

				try{
					callBackInfo = XMLUtils.stringToElement(clientData);
				}catch(Exception e){ //if clientdata is not empty and not in th format we expect, ignore.
					device.getLogger().error("Error while client call back data from PublishFileResponseHandler : ", e);
					return;
				}				
				Integer index = Integer.parseInt(XMLUtils.getAttributeValueWithDefault(callBackInfo, "index","0"));	
				if(index!=0){
					String dirID = XMLUtils.getAttributeValueWithDefault(callBackInfo, "dirID","");
					String dirNID = XMLUtils.getAttributeValueWithDefault(callBackInfo, "dirNID","");
					String pvrDirNID = XMLUtils.getAttributeValueWithDefault(callBackInfo, "pvrDirNID","");
					String recyclebinDirNID = XMLUtils.getAttributeValueWithDefault(callBackInfo, "recyclebinDirNID","");
					String mediaStreamFileMaxRevision = XMLUtils.getAttributeValueWithDefault(callBackInfo, "mediaStreamFileMaxRevision","");
					device.getMediaStoreAgentImpl().startSynch(dirID, mediaStreamFileMaxRevision, dirNID, pvrDirNID, recyclebinDirNID, index);
				}
			} catch(Exception e){
				device.getLogger().error("Error parsing publish file response from PublishFileResponseHandler: ", e);

			}
		}
	}

}
