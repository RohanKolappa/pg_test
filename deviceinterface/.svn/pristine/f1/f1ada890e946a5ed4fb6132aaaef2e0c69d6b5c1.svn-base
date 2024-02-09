package com.barco.device.handler.DeviceAdmin;

import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.StreamEntry;
import com.barco.device.handler.Layout.TeardownRequestHandler;
import com.barco.utils.XMLUtils;

public class DeviceConfigUpdateEventHandler extends BaseEventHandler {

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {

		Document eventDoc = message.getMessageDoc();
		
		//handle port type changed
		handlePortTypeChanged(eventDoc,device);

		try {
			//call media agent to handle config update 
		    if(device.getMediaAgentImpl()!=null)
			device.getMediaAgentImpl().handleConfigUpdated(eventDoc);

			//call layout agent to handle config update		
			if(device.getLayoutAgentImpl()!=null)
				device.getLayoutAgentImpl().handleConfigUpdated(eventDoc);
		} catch (Exception e) {
		   device.getLogger().error("DeviceConfigUpdateEventHandler: Hanlding event failed.",e);
		}

		super.handleDeviceUpdateEvent(message, device, "SetServiceClientConfig","DeviceConfig");
	}	

	private void handlePortTypeChanged(Document eventDoc, DeviceI device) {


		try {
			String newPortType = XMLUtils.getValueXpath(eventDoc, "//Info/PortIOType","");
			if(newPortType.isEmpty()){
				return;
			}		

			String portIDXpath = XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpath(eventDoc, "//DeviceConfig"), "xpath", "");

			//get port id from XPath
			String regex = "../PortID\\s*=\\s*(.*?)]";
			Matcher matcher = Pattern.compile(regex).matcher(portIDXpath);
			String portID = "";
			if(matcher.find()) {
				portID= matcher.group(1);
			}

			if(portID == null || portID.isEmpty()) {
				return;
			}

			device.getLogger().debug("DeviceConfigUpdateEventHandler:handlePortTypeChanged portId:="+portID+ " newType:="+newPortType);

			
			if(newPortType.equals("StreamSrc")){
				StreamEntry streamEntry = device.getDeviceDB().getStreamCache().getStreamByDestPortElementID(portID);
				if(streamEntry == null) {
					return;
				}
				device.getLogger().debug("DeviceConfigUpdateEventHandler:handlePortTypeChanged Stop stream "+streamEntry.streamNID);

				//update stream state as setup error
				String exceptionMessage = new ServiceException(MediaAgentI.SETUP_ERROR, "Port Type Changed").toString();
				Element exceptionEl = XMLUtils.stringToElement(exceptionMessage);

				//TBR: Backward compatibility on SC
				exceptionEl.setName("Exception");
				Element errorCode  = DocumentHelper.createElement("Code");
				errorCode.setText(String.valueOf(MediaAgentI.SETUP_ERROR));
				exceptionEl.add(errorCode);
				streamEntry.status.add(exceptionEl);

				streamEntry.setStatus(MediaAgentI.SETUP_ERROR, device, true);

				TeardownRequestHandler handler = new TeardownRequestHandler();
				handler.teardownStream(device, streamEntry.streamNID);

			} else {
				//get list of streams based on port id
				List<String>  streamNIDList = device.getMediaAgentImpl().getStreamNIDListByPortID(portID);
				if(streamNIDList ==null || streamNIDList.isEmpty()) {
					return;
				}

				device.getLogger().debug("XPMediaAgentImpl:handleConfigUpdated StreamList:= " +streamNIDList);

				for(String streamNID : streamNIDList) {
					device.getMediaAgentImpl().stopStream(streamNID);
				}

			}
		} catch (Exception e) {
			//ignore exception flag log message
			device.getLogger().error("DeviceConfigUpdateEventHandler: Handling of Port type Changed failed.", e);
		}
	}
}
