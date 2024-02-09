package com.ipvs.mediaroomservice.stream;

import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;

public class DeviceTable {
	Document deviceInstanceListDoc;
	String deviceType;
	String userJID;
	MRLog log;
	private Hashtable<String, String> deviceInstanceTypeMap = null;

	private static final String INSTANCE_LIST_NID = "DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList";

	public DeviceTable(String userJID, String deviceType, MRLog log) {
		this.deviceType = deviceType;
		this.userJID = userJID;
		this.log = log;
		try {
			this.deviceInstanceListDoc = MRXMLUtils
					.stringToDocument("<DeviceInstanceList userJID='"
							+ this.userJID + "'/>");
		} catch (Exception e) {
			e.printStackTrace();
		}
		this.deviceInstanceTypeMap = DeviceTable.getDeviceInstanceTypeMap();
	}

	public String getDeviceType() {
		return deviceType;
	}

	public void setDeviceType(String deviceType) {
		this.deviceType = deviceType;
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

		return typeMap;
	}

	private String getInstanceElement(String type) {
		return deviceInstanceTypeMap.get(type);
	}

	private String generateDeviceInstanceNID(String deviceNID,
			String ssInstanceID) throws MRException, Exception {
		return deviceNID + "." + INSTANCE_LIST_NID + "." + ssInstanceID;
	}

	private synchronized String addDeviceInstance(String deviceNID,
			String ssInstanceNID, String instanceType) throws Exception {
		//perf-logs this.log.addInfo(this.userJID + ": SourceTable: addInstanceInDB - Start", MRLog.OK, MRLog.NOTIFY);
		String deviceInstanceNID = generateDeviceInstanceNID(deviceNID,
				ssInstanceNID);
		Document instanceDoc = AssetAdminServiceUtils.getInstanceListDocument(
				getDeviceType(), log);		

		String instanceName = getInstanceElement(instanceType);

		Element deviceInstanceElement = MRXMLUtils.getElementXpath(instanceDoc,
				"//InstanceList/" + instanceName);
		MRXMLUtils.getElementXpath(instanceDoc,
				"//" + instanceName + "/SSInstanceID").setText(ssInstanceNID);
		MRXMLUtils
				.addAttribute(deviceInstanceElement, "NID", deviceInstanceNID);
		this.deviceInstanceListDoc.getRootElement().add(
				deviceInstanceElement.detach());

		return deviceInstanceNID;
	}

	private synchronized void deleteDeviceInstance(String deviceInstanceNID,
			String instanceType) throws Exception {
		String instanceName = getInstanceElement(instanceType);
		String xpath = "//" + instanceName + "[@NID='" + deviceInstanceNID
				+ "']";
		Element deviceInstance = MRXMLUtils.getElementXpath(
				deviceInstanceListDoc, xpath);

		if (deviceInstance != null) {
			deviceInstanceListDoc.getRootElement()
					.remove(deviceInstance);
		} else {
			// System.out.println("SourceTable::deleteDeviceInstance: Not Found xpath=" + xpath + " DeviceInstanceNID=" + deviceInstanceNID);
		}

		// System.out.println(MRXMLUtils.documentToString(deviceInstanceListDoc));
	}

	public String addInputInstanceNID(String deviceNID, Element inputElement)
			throws Exception {
		String ssInstanceNID = MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID");
		String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");

		if (deviceType == null) {
			return ssInstanceNID;
		}


		return addDeviceInstance(deviceNID, ssInstanceNID, inputType);
	}

	public void deleteInputInstanceNID(String deviceNID, String instanceNID,
			Element inputElement) throws Exception {
		if (deviceType == null) {
			return;
		}

		String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
		deleteDeviceInstance(instanceNID, inputType);
	}

	public String addOutputInstanceNID(String deviceNID, Element outputElement)
			throws Exception {
		String ssInstanceNID = MRXMLUtils.getAttributeValue(outputElement,"ssInstanceID");
		String outputType = MRXMLUtils.getAttributeValue(outputElement,"outputType");

		if (deviceType == null) {
			return ssInstanceNID;
		}
		
		return addDeviceInstance(deviceNID, ssInstanceNID, outputType);
	}

	public void deleteOutputInstanceNID(String deviceNID, String instanceNID,
			Element outputElement) throws Exception {
		if (deviceType == null) {
			return;
		}

		String outputType = MRXMLUtils.getAttributeValue(outputElement,"outputType");
		deleteDeviceInstance(instanceNID, outputType);
	}

}
