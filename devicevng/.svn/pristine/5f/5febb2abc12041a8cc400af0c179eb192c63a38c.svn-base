package com.barco.devicevng.MediaStore.responsehandlers;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.handler.DeviceAdmin.SCServiceReadyEventHandler;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.handler.MediaStore.MediaStoreUtils;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class PublishDirResponseHandler implements ResponseHandlerI {

	@Override
	public void handleResponse(Message response, Document context, DeviceI device) throws Exception {
		Document messageDoc = response.getMessageDoc();
		//update directory information
		Element dirElement = XMLUtils.getElementXpath(messageDoc, "//Dir");         
		if(dirElement == null) {
			return;
		}
		//fill  dir information in cache
		String dirId = XMLUtils.getAttributeValueWithDefault(dirElement, "id","");
		String dirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "dirNID","");
		String pvrDirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "pvrDirNID","");
		String recycleDirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "recyclebinDirNID","");
		String dirThreshold = XMLUtils.getAttributeValueWithDefault(dirElement, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD,"");
		String state = XMLUtils.getAttributeValueWithDefault(dirElement, "state","");

		device.getMediaStoreAgentImpl().updateDBInformation(dirId, dirNID, pvrDirNID, recycleDirNID, Utils.getLongValue(dirThreshold, 0), state);
		device.getLogger().debug("Update Directory inforamtion in Media Store Cache ID:="+dirId +" state:="+state);
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device) throws Exception {

		device.getLogger().error("PublishDirResponseHandler: Publish Dir Response Failed" +response.getMessageDoc().asXML());
		Document messageDoc = response.getMessageDoc();
		String code = XMLUtils.getValueXpath(messageDoc, "//Code", "");
		//handle the case where we need to start sync
		if(code.equals("1042")){
			Element dirElement = XMLUtils.getElementXpath(response.getMessageDoc(), "//Log/.//Dir");
			device.getLogger().debug("PublishDirResponseHandler: PublishDir Request failed. Start Sync process.");
			String dirId = XMLUtils.getAttributeValueWithDefault(dirElement, "id","");
			new SCServiceReadyEventHandler ().startSyncProcess(dirElement, device); 
			Document publishDirInfoDoc = device.getMediaStoreAgentImpl().getPublishInfo();
			Element publishDirInfoEl = XMLUtils.getElementXpath(publishDirInfoDoc, "//Dir[@id='" + dirId + "']");
			MediaStoreUtils.sendPublishDirRequest(publishDirInfoEl,device,new PublishDirResponseHandler());

		} else {
			device.getLogger().debug("PublishDir Request failed. Reason:"+XMLUtils.getValueXpath(messageDoc, "//Description", ""));
		}
	}

}


