package com.barco.devicevng.MediaStore.responsehandlers;

import org.dom4j.Document;

import com.barco.device.base.DeviceI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class PublishDirPurgeFileStatusResponseHandler implements ResponseHandlerI {

	@Override
	public void handleResponse(Message response, Document context, DeviceI device) throws Exception {
	      device.getLogger().debug("PublishDirPurgeFileStatus Request passed.");
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device) throws Exception {
	      device.getLogger().debug("PublishDirPurgeFileStatus Request failed. Reason:"+XMLUtils.getValueXpath(response.getMessageDoc(), "//Description", ""));
	}

}


