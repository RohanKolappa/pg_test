package com.ipvs.sessionservice.features;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.PortBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class IPVSClientDisplayConfig extends FeatureBase{
	
	public Document generateWindowAPIFromDeviceConfig(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document portDisplayConfigDoc, Element portElement) throws Exception {
		String elementName = SessionServiceConst.IPVSCLIENTDISPLAYCONFIG;
		String displayName = SessionServiceConst.DISPLAY;
		String canvasName = SessionServiceConst.CANVAS;
		Document deviceResultDoc = PortBaseRequestHandler.getDeviceDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, portElement, 
				"//" + elementName,
				SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE, 
				SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE);
		
		Element displayElement = MRXMLUtils.getElementXpath(deviceResultDoc, "//" + elementName + "/" + displayName);
		Element canvasElement = MRXMLUtils.getElementXpath(deviceResultDoc, "//" + elementName + "/" + canvasName);
				
		MRXMLUtils.removeContent(portDisplayConfigDoc.getRootElement());
		if(displayElement != null)
			portDisplayConfigDoc.getRootElement().add(displayElement.detach());
		if(canvasElement != null)
			portDisplayConfigDoc.getRootElement().add(canvasElement.detach());
		
		return portDisplayConfigDoc;
	}
		
	public Document generateWindowAPIFromDeviceStatus(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document portDisplayStatusDoc, Element portElement) throws Exception {
		// String elementName = SessionServiceConst.IPVSCLIENTDISPLAYSTATUS;
		// TBR once the ClientUI starts updating the Status
		String elementName = SessionServiceConst.IPVSCLIENTDISPLAYSTATUS;
		String displayName = SessionServiceConst.DISPLAY;
		String canvasName = SessionServiceConst.CANVAS;
		Document deviceResultDoc = PortBaseRequestHandler.getDeviceDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, portElement, 
				"//" + elementName, 
				SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE, 
				SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE);

		Element displayElement = MRXMLUtils.getElementXpath(deviceResultDoc, "//" + elementName + "/" + displayName);
		Element canvasElement = MRXMLUtils.getElementXpath(deviceResultDoc, "//" + elementName + "/" + canvasName);
	
		MRXMLUtils.removeContent(portDisplayStatusDoc.getRootElement());
		portDisplayStatusDoc.getRootElement().add(displayElement.detach());
		portDisplayStatusDoc.getRootElement().add(canvasElement.detach());
	
		return portDisplayStatusDoc;
	}
	
	public Document generateDeviceAgentRequestFromWindowAPI(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element setPortDisplayConfigElement, Element portElement) throws Exception {
		
		// Generate the avCodecElementsConfig
    	Element canvas = (Element)setPortDisplayConfigElement.element("Canvas").detach();
    	Element avCodecElementsConfig = MRXMLUtils.newElement("AVCodecElementsConfig");
    	Element displayConfig = MRXMLUtils.newElement(SessionServiceConst.IPVSCLIENTDISPLAYCONFIG);
    	avCodecElementsConfig.add(displayConfig);
    	displayConfig.add(canvas);

    	// Update the DeviceRequest
    	Document deviceConfigurationRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/systemservice/xml/DeviceConfigurationRequest.xml", log);
		MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, "//DeviceConfig").clearContent();
		MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, "//DeviceConfig").add(avCodecElementsConfig.detach());
		
		/*// Update the Device
		String portNID = portElement.attributeValue(SessionServiceConst.NID);
		String whereNIDXPath = GetObjectUtils.generateXPath(portNID, 
				SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE, 
				SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE, 
				SessionServiceConst.PORT_CONST);
        String selectXML = "<Device>" +"{root($VAR)/\*\/@NID}" + "</Device>";
    	Element responseEL = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.Device,selectXML, whereNIDXPath, request, db, log, xmpp);    	
		String deviceNID = responseEL.element("Device").attributeValue("NID");
        String childXpath = "//" + SessionServiceConst.IPVSCLIENTDISPLAYCONFIG;
		AssetAdminServiceUtils.updateChild(deviceNID, childXpath, displayConfig, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		 */
		return deviceConfigurationRequestDoc;
	}

	public Element getDisplayStatusElement(Element sourceElement, MRLog log) throws Exception {		
		
		return sourceElement;
	}
	
	
	@Override
	public Element getDisplayConfigElement(Element sourceElement, MRLog log)
			throws Exception {
		return  sourceElement;
	}

	@Override
	public void updateConfigInDatabase(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document  deviceConfigurationRequestDoc,Element portElement)
			throws Exception {
		String deviceElName = SessionServiceConst.IPVSCLIENTDISPLAYCONFIG;
		
		String displayXPath =  "//"+deviceElName+"/"+SessionServiceConst.DISPLAY;
		String canvasXPath = "//"+deviceElName+"/"+SessionServiceConst.CANVAS;
	
		String portObjectType = SessionServiceConst.MEDIA_STREAM_DST_PORT_RESOURCE;
		
		
		Element displayEl = MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, displayXPath );
		Element canvasEl = MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, canvasXPath );
		
		
		String updateChildXpath = "//" + deviceElName;	
		if(displayEl != null && canvasEl == null) {
			updateChildXpath = displayXPath;	
		} else if(displayEl == null && canvasEl != null) {
			updateChildXpath = canvasXPath;
		}	
     	super.updateInDatabase(request, xmpp, log, db, resourceFactory, serviceAgentFactory, deviceConfigurationRequestDoc, portElement, portObjectType, updateChildXpath);	

	}
}
