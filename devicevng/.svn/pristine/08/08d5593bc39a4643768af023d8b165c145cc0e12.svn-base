package com.barco.device.vng.Media;

import java.util.Hashtable;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;


public class DeviceTable {
	Document deviceInstanceListDoc;
	Logger log;
	DeviceDBI deviceDB;
	private Hashtable<String, String> deviceInstanceTypeMap = null;

	private static final String INSTANCE_LIST_NID = "DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList";

	public DeviceTable(Logger log, DeviceDBI deviceDB) {
		this.log = log;
		this.deviceDB = deviceDB;
		try {
			this.deviceInstanceListDoc = XMLUtils
					.stringToDocument("<DeviceInstanceList userJID='"
							+ this.deviceDB.getDeviceResourceNID() + "'/>");
		} catch (Exception e) {
			e.printStackTrace();
		}
		this.deviceInstanceTypeMap = DeviceTable.getDeviceInstanceTypeMap();
	}

	private static Hashtable<String, String> getDeviceInstanceTypeMap() {
		Hashtable<String, String> typeMap = new Hashtable<String, String>();
		typeMap.put(MediaRoomServiceConst.INPUT_RELAY,
				MediaRoomServiceConst.V2D_NETWORK_DEVICE_INSTANCE);
		typeMap.put(MediaRoomServiceConst.INPUT_MEDIAFILE,
				MediaRoomServiceConst.V2D_NETWORK_DEVICE_INSTANCE);
		typeMap.put(MediaRoomServiceConst.INPUT_ENCODER,
				MediaRoomServiceConst.V2D_ENCODER_DEVICE_INSTANCE);
		typeMap.put(MediaRoomServiceConst.OUTPUT_DECODER,
				MediaRoomServiceConst.V2D_DECODER_DEVICE_INSTANCE);
		typeMap.put(MediaRoomServiceConst.OUTPUT_PVR,
				MediaRoomServiceConst.DMS_RECORDED_DEVICE_INSTANCE);
		typeMap.put(MediaRoomServiceConst.OUTPUT_RECORD,
				MediaRoomServiceConst.DMS_RECORDED_DEVICE_INSTANCE);
		typeMap.put(MediaRoomServiceConst.OUTPUT_STREAM,
				MediaRoomServiceConst.V2D_STREAM_TX_INSTANCE);
		typeMap.put(MediaRoomServiceConst.INPUT_STREAM,
				MediaRoomServiceConst.V2D_STREAM_RX_INSTANCE);
		typeMap.put(MediaRoomServiceConst.OUTPUT_IMAGE_WRITER,
			MediaRoomServiceConst.IMAGE_WRITER_INSTANCE);

		return typeMap;
	}

	private String getInstanceElement(String type) {
		return deviceInstanceTypeMap.get(type);
	}

	private String generateDeviceInstanceNID(String deviceNID,
			String ssInstanceID) throws ServiceException, Exception {
		return deviceNID + "." + INSTANCE_LIST_NID + "." + ssInstanceID;
	}

	private synchronized String addDeviceInstance(String deviceNID,
			String ssInstanceNID, String instanceType) throws Exception {
		
		//perf-logs this.log.addInfo(this.userJID + ": SourceTable: addInstanceInDB - Start", MRLog.OK, MRLog.NOTIFY);
		String deviceInstanceNID = generateDeviceInstanceNID(deviceNID,ssInstanceNID);

		String instanceName = getInstanceElement(instanceType);				
		Element deviceInstanceElement = (Element)XMLUtils.getElementXpath(this.deviceDB.getDeviceConfig(), "//InstanceList/" + instanceName).clone();
		deviceInstanceElement.element("SSInstanceID").setText(ssInstanceNID);
		XMLUtils.addAttribute(deviceInstanceElement, "NID", deviceInstanceNID);
		this.deviceInstanceListDoc.getRootElement().add(deviceInstanceElement);

		return deviceInstanceNID;
	}

	private synchronized void deleteDeviceInstance(String deviceInstanceNID,
			String instanceType) throws Exception {
		String instanceName = getInstanceElement(instanceType);
		String xpath = "//" + instanceName + "[@NID='" + deviceInstanceNID
				+ "']";
		Element deviceInstance = XMLUtils.getElementXpath(deviceInstanceListDoc, xpath);

		if (deviceInstance != null) {
			deviceInstanceListDoc.getRootElement().remove(deviceInstance);
		} else {
			// System.out.println("SourceTable::deleteDeviceInstance: Not Found xpath=" + xpath + " DeviceInstanceNID=" + deviceInstanceNID);
		}

		// System.out.println(MRXMLUtils.documentToString(deviceInstanceListDoc));
	}

	public String addInputInstanceNID(String deviceNID, Element inputElement)
			throws Exception {
		String ssInstanceNID = XMLUtils.getAttributeValue(inputElement,"ssInstanceID");
		String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");

		if (this.deviceDB.getDeviceType() == null) {
			return ssInstanceNID;
		}


		return addDeviceInstance(deviceNID, ssInstanceNID, inputType);
	}

	public void deleteInputInstanceNID(String deviceNID, String instanceNID,
			Element inputElement) throws Exception {
		if (this.deviceDB.getDeviceType() == null) {
			return;
		}

		String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
		deleteDeviceInstance(instanceNID, inputType);
	}

	public String addOutputInstanceNID(String deviceNID, Element outputElement)
			throws Exception {
		String ssInstanceNID = XMLUtils.getAttributeValue(outputElement,"ssInstanceID");
		String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");

		if (this.deviceDB.getDeviceType() == null) {
			return ssInstanceNID;
		}
		
		return addDeviceInstance(deviceNID, ssInstanceNID, outputType);
	}

	public void deleteOutputInstanceNID(String deviceNID, String instanceNID,
			Element outputElement) throws Exception {
		if (this.deviceDB.getDeviceType() == null) {
			return;
		}

		String outputType = XMLUtils.getAttributeValue(outputElement,"outputType");
		deleteDeviceInstance(instanceNID, outputType);
	}

}
