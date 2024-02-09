package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;


public abstract class BaseEventHandler implements MessageHandlerI,ResponseHandlerI {
	
	public void handleDeviceUpdateEvent(Message message, DeviceI device,String requestName,String updateName) throws Exception {
		if(device.getDeviceDB().getDeviceXMLNID() == null) {
			return;
		}
		Document requestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/device/xml/"+requestName+"Request.xml"); 
		Element requestElement = (Element) DocumentHelper.createXPath("//"+requestName).selectSingleNode(requestDoc);
		Element updateEventEl = XMLUtils.getElementXpath(message.getMessageDoc(), "//"+updateName);
		String operation = XMLUtils.getAttributeValueWithDefault(updateEventEl, "operation", "");
		String xpath = XMLUtils.getAttributeValueWithDefault(updateEventEl, "xpath", "");
		String thumbnailSupport = XMLUtils.getAttributeValueWithDefault(updateEventEl, "thumbnail", "false");
		requestElement.addAttribute("xPath", xpath);
		requestElement.addAttribute("operation", operation);
		if(thumbnailSupport.equals("true")){
		   requestElement.addAttribute("thumbnail", thumbnailSupport);   
		}
		requestElement.addAttribute("byNID", device.getDeviceDB().getDeviceXMLNID());
		if(operation.equals("Add") || operation.equals("Update")) {
            requestElement.add(((Element)updateEventEl.content().get(0)).detach());
        }
		if(operation.equals("UpdateAttribute")){
			Element attributeEl = XMLUtils.getElementXpathFromElement(updateEventEl, "./Attribute");
			String attributeName = XMLUtils.getAttributeValueWithDefault(attributeEl, "name", "");
			String attributeValue =XMLUtils.getAttributeValueWithDefault(attributeEl, "value", "");
			if(attributeName.isEmpty()) {
				return;
			}
			requestElement.setText("attribute "+attributeName+" {\""+attributeValue+"\"}");
		}
		Message requestMessage = Message.createRequest(requestDoc, device.getAppserverJID(), device.getMyJID());
		device.sendRequest(requestMessage, requestDoc, this);				
	}
	
    protected void relogin(String xmppServerIp, String deviceName,String devicePassword,DeviceI device) {
		
		//logout logged use
		device.logout();
		
		try {
			device.login(xmppServerIp, deviceName, devicePassword, deviceName, device.getLoginInfo().getAppserverJID());
		} catch (Exception e) {
			e.printStackTrace();
			device.getLogger().error("EventHandler: XMPP Login Failed \n"+e.toString());
		}
		
	}
	
	
	@Override
	public void handleResponse(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device)
			throws Exception {
		// TODO Auto-generated method stub
		
	}
}
