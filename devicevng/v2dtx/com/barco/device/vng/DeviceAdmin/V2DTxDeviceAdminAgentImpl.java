package com.barco.device.vng.DeviceAdmin;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;

public class V2DTxDeviceAdminAgentImpl extends BaseDeviceAdminAgentImpl {
	
	public V2DTxDeviceAdminAgentImpl() {
		super();
	}

	@Override
	protected List<Element> getPortResources(Document configDocument, Document statusDocument) throws Exception {
		List<Element> list  = new ArrayList<Element>();
		Element portElement= DocumentHelper.createElement(DeviceAdminConst.PORT_ELEMENT_NAME);
		portElement.addAttribute(DeviceAdminConst.TYPE_ATTRIBUTE_NAME, DeviceAdminConst.SRC_PORT);
		portElement.addAttribute(DeviceAdminConst.ID_ATTRIBUTE_NAME,"1");
		portElement.addAttribute(DeviceAdminConst.MAX_INSTANCE_COUNT_ATTRIBUTE_NAME,"1");
		portElement.addAttribute(DeviceAdminConst.STREAM_TYPE_ATTRIBUTE,"V2D");
		portElement.addAttribute(DeviceAdminConst.THUMBNAIL,"false");
		list.add(portElement);
		return list;
	}

	@Override
	public void handleConfigUpdate(DeviceI device, Element activeServerConfigEl) throws Exception {
	    
	}
	

	public void haTakeover() throws Exception {
		// TODO Auto-generated method stub
		
	}	

}
