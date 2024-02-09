package com.barco.device.vng.DeviceAdmin;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.utils.XMLUtils;

public class DMSDeviceAdminAgentImpl extends BaseDeviceAdminAgentImpl{

	public DMSDeviceAdminAgentImpl() {
		super();
	}

	@Override
	protected List<Element> getPortResources(Document configDocument,
			Document statusDocument) throws Exception {
		List<Element> list  = new ArrayList<Element>();
		Element portElement= DocumentHelper.createElement(DeviceAdminConst.PORT_ELEMENT_NAME);
		portElement.addAttribute(DeviceAdminConst.TYPE_ATTRIBUTE_NAME, DeviceAdminConst.RELAY_PORT);
		portElement.addAttribute(DeviceAdminConst.ID_ATTRIBUTE_NAME,"1");
		portElement.addAttribute(DeviceAdminConst.STREAM_TYPE_ATTRIBUTE,"V2D,MPEG,RTP,UDP");
		portElement.addAttribute(DeviceAdminConst.MAX_INSTANCE_COUNT_ATTRIBUTE_NAME,"25");
		list.add(portElement);
		
		portElement= DocumentHelper.createElement(DeviceAdminConst.PORT_ELEMENT_NAME);
		portElement.addAttribute(DeviceAdminConst.TYPE_ATTRIBUTE_NAME, DeviceAdminConst.STORE_PORT);
		portElement.addAttribute(DeviceAdminConst.ID_ATTRIBUTE_NAME,"1");
		portElement.addAttribute(DeviceAdminConst.MAX_INSTANCE_COUNT_ATTRIBUTE_NAME,"25");
		portElement.addAttribute(DeviceAdminConst.STREAM_TYPE_ATTRIBUTE,"V2D,MPEG,RTP,UDP");
		portElement.add((Element)XMLUtils.getElementXpath(statusDocument, "//HardDiskTableStatus").clone());
		
		list.add(portElement);
		return list;
	}

	@Override
	public void handleConfigUpdate(DeviceI device, Element activeServerConfigEl) throws Exception {
	    
	}

	public void haTakeover() throws Exception {
		// TODO Auto-generated method stub
		
	}
	    // TODO Auto-generated method stub
	    
}
