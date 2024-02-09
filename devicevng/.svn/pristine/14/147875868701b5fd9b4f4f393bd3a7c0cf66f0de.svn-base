package com.barco.device.vng.DeviceAdmin;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.utils.XMLUtils;

public class XPDeviceAdminAgentImpl extends BaseDeviceAdminAgentImpl{

	public XPDeviceAdminAgentImpl() {
		super();
	}

	@Override
	protected List<Element> getPortResources(Document configDocument,
			Document statusDocument) throws Exception {
		List<Element> list  = new ArrayList<Element>();
		List<?> listOfPorts  = XMLUtils.getListOfNodesXpath(configDocument, ".//XP200PortConfig");
		for(int portIndex=1;portIndex<=listOfPorts.size();portIndex++){
			Element portElement= DocumentHelper.createElement(DeviceAdminConst.PORT_ELEMENT_NAME);
			String portType = DeviceAdminConst.SRC_PORT;
			try {
				if(XMLUtils.getValueXpath(configDocument, "//XP200PortConfig[PortID='"+portIndex+"']/Info/PortIOType", DeviceAdminConst.SRC_PORT).equals("StreamDst")){
					portType = DeviceAdminConst.DST_PORT;
					Element displayConfig = DocumentHelper.createElement("DisplayConfig");
					Element displayStatus = DocumentHelper.createElement("DisplayStatus");
					displayConfig.add((Element)XMLUtils.getElementXpath(configDocument, "//XP200PortConfig[PortID='"+portIndex+"']/.//XP200DisplayConfig").clone());
					displayStatus.add((Element)XMLUtils.getElementXpath(statusDocument, "//XP200PortStatus[PortID='"+portIndex+"']/.//XP200DisplayStatus").clone());
					portElement.add(displayConfig);
					portElement.add(displayStatus);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
			portElement.addAttribute(DeviceAdminConst.TYPE_ATTRIBUTE_NAME, portType);
			portElement.addAttribute(DeviceAdminConst.ID_ATTRIBUTE_NAME,String.valueOf(portIndex));
			portElement.addAttribute(DeviceAdminConst.MAX_INSTANCE_COUNT_ATTRIBUTE_NAME,"1");
			portElement.addAttribute(DeviceAdminConst.STREAM_TYPE_ATTRIBUTE,"V2D");
			if(portType.equals(DeviceAdminConst.SRC_PORT)) {
                portElement.addAttribute(DeviceAdminConst.THUMBNAIL,"true");
			}
			list.add(portElement);
		}
		return list;
	}

	@Override

	public void handleConfigUpdate(DeviceI device, Element activeServerConfigEl) throws Exception {
	    
	}

	public void haTakeover() throws Exception {
		// TODO Auto-generated method stub
		
	}

}
