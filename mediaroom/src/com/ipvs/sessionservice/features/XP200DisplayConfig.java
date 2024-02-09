package com.ipvs.sessionservice.features;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.ConfigDeviceUtils;
import com.ipvs.sessionservice.handler.base.PortBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.XSLTUtils;

public class XP200DisplayConfig extends FeatureBase{
	private static final String SET_XP_200_DISPLAY_CONFIG = "SetXP200DisplayConfig.xml";
	private static final String GET_XP_200_DISPLAY_CONFIG = "GetXP200DisplayConfig.xml";
	private static final String GET_XP_200_DISPLAY_STATUS = "GetXP200DisplayStatus.xml";
	
	public Document generateWindowAPIFromDeviceConfig(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document portDisplayConfigDoc, Element portElement) throws Exception {
		String portID = portElement.attributeValue("deviceElementID");		
		String deviceElementName = "//XP200PortConfig[PortID = \'" + portID + "\']"+"/"+SessionServiceConst.XP200DISPLAYCONFIG;
		Document deviceResultDoc = PortBaseRequestHandler.getDeviceDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, portElement, deviceElementName,
				SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE, SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE);

		Element xp200DisplayConfig = MRXMLUtils.getElementXpath(deviceResultDoc, "//" + SessionServiceConst.XP200DISPLAYCONFIG);

		//translate the device config into window api format
		xp200DisplayConfig = this.getDisplayConfigElement(xp200DisplayConfig, log);

		MRXMLUtils.removeContent(portDisplayConfigDoc.getRootElement());
		portDisplayConfigDoc.getRootElement().add(xp200DisplayConfig.detach());

		return portDisplayConfigDoc;
	}
	
	
	public Document generateWindowAPIFromDeviceStatus(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document portDisplayStatusDoc, Element portElement) throws Exception {
		String portID = portElement.attributeValue("deviceElementID");		
		String deviceElementName = "//XP200PortStatus[PortID = \'" + portID + "\']" +"/"+SessionServiceConst.XP200DISPLAYSTATUS;
		Document deviceResultDoc = PortBaseRequestHandler.getDeviceDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, portElement, deviceElementName,
				SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE, SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE);
		Element xp200DisplayStatus = MRXMLUtils.getElementXpath(deviceResultDoc, "//"+ SessionServiceConst.XP200DISPLAYSTATUS);
		xp200DisplayStatus = this.getDisplayStatusElement(xp200DisplayStatus, log);
		
		MRXMLUtils.removeContent(portDisplayStatusDoc.getRootElement());
		portDisplayStatusDoc.getRootElement().add(xp200DisplayStatus.detach());
		return portDisplayStatusDoc;
	}
	
	
	public Document generateSetPortDisplayConfigDeviceRequest (MRRequest request, MRLog log, Element setPortDisplayConfigElement, Element portEl) throws Exception {
		
		//List<?> portConfigElementChildren = MRXMLUtils.getChildren(setPortDisplayConfigElement);
	
		Element canvasEL = MRXMLUtils.getElementXpath(setPortDisplayConfigElement, "//Canvas");
		Element xp200DisplayConfig  = XSLTUtils.executeTranslateCommand(SessionServiceConst.FEATURES_XSL_PATH+ SET_XP_200_DISPLAY_CONFIG, canvasEL,log);
		
		//(Element)MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(portEl), "//XP200DisplayConfig").clone();
		
		Element portIDElement = (Element)MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(portEl), "//PortID").clone();
		
		/*for(int i=0;i<portConfigElementChildren.size();i++) {
    		String portConfigElementChildName = ((Element)portConfigElementChildren.get(i)).getName();
    		String portConfigElementChildValue = ((Element)portConfigElementChildren.get(i)).getText();
    		MRXMLUtils.setValueXpath(xp200DisplayConfig, portConfigElementChildName, portConfigElementChildValue);
    	}*/
		
    	MRXMLUtils.addAttribute(setPortDisplayConfigElement, ConfigDeviceUtils.DIRTY_TAG, ConfigDeviceUtils.MODIFY_FLAG);
    	MRXMLUtils.setAttributeValue(portEl, ConfigDeviceUtils.DIRTY_TAG, ConfigDeviceUtils.MODIFY_FLAG);
    	portEl.clearContent();
    	portEl.add(portIDElement);
    	portEl.add(xp200DisplayConfig);
    	Element avCodecElementsConfig = MRXMLUtils.newElement("AVCodecElementsConfig");
    	avCodecElementsConfig.add(portEl.detach());
    	
    	Element deviceConfig = MRXMLUtils.newElement("DeviceConfig");
    	MRXMLUtils.addElement(deviceConfig,(Element) avCodecElementsConfig.detach());
    	
    	Element setDeviceConfigElement = MRXMLUtils.newElement("SetDeviceConfigRequestData");
    	MRXMLUtils.addElement(setDeviceConfigElement,deviceConfig);
    	MRXMLUtils.addAttribute(setDeviceConfigElement, "doNotSaveToFlash", SessionServiceConst.TRUE);
    	String requestString = MRClientAgentUtils.formatRequestString(MRConst.DEVICE_ADMIN_SERVICE, "SetDeviceConfigRequest", request.getAppServerJID(), "", "", "", setDeviceConfigElement.asXML(), "false");
  	
		return DocumentHelper.parseText(requestString);
	}
	
	public Document generateDeviceAgentRequestFromWindowAPI(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element setPortDisplayConfigElement, Element portElement) throws Exception {
		
		String portID = portElement.attributeValue("deviceElementID");
		String deviceElementName = "//XP200PortConfig[PortID = \'" + portID + "\']";		
		Document deviceResultDoc = PortBaseRequestHandler.getDeviceDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, portElement, deviceElementName, 
				SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE, SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE);
		Element xp200PortConfig = MRXMLUtils.getElementXpath(deviceResultDoc, "//XP200PortConfig");
		Document deviceConfigurationRequestDoc = generateSetPortDisplayConfigDeviceRequest(request, log, setPortDisplayConfigElement, xp200PortConfig );
		return deviceConfigurationRequestDoc;
	}


	@Override
	public Element getDisplayStatusElement(Element sourceElement, MRLog log)
			throws Exception {
		return  XSLTUtils.executeTranslateCommand(SessionServiceConst.FEATURES_XSL_PATH+GET_XP_200_DISPLAY_STATUS, sourceElement,log);
	}


	@Override
	public Element getDisplayConfigElement(Element sourceElement, MRLog log)
			throws Exception {
		return  XSLTUtils.executeTranslateCommand(SessionServiceConst.FEATURES_XSL_PATH+GET_XP_200_DISPLAY_CONFIG, sourceElement,log);
	}


	@Override
	public void updateConfigInDatabase(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Document  deviceConfigurationRequestDoc, Element portElement)
			throws Exception {
		String deviceElName = SessionServiceConst.XP200DISPLAYCONFIG;
		String portID = portElement.attributeValue("deviceElementID");		
		String updateChildXpath = "//"+deviceElName+"[../PortID = \'" + portID + "\']";		
		String portObjectType = SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE;
		super.updateInDatabase(request, xmpp, log, db, resourceFactory, serviceAgentFactory, deviceConfigurationRequestDoc, portElement, portObjectType,  updateChildXpath);	
	}
	
}
