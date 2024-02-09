package com.barco.device.handler.Media;

import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class SSEventHandler implements MessageHandlerI{

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Element ssEvent = XMLUtils.getElementXpath(message.getMessageDoc(), "//SSEvent");
		
		String state = XMLUtils.getAttributeValueWithDefault(ssEvent, "state","");
		if(state.equals("Connected" )){
			device.getMediaAgentImpl().handleConnected(message.getMessageDoc());			
		}
	}
}
