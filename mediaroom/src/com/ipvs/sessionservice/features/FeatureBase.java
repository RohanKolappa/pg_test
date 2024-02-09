package com.ipvs.sessionservice.features;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.PortBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public abstract class FeatureBase {

	public abstract Document generateWindowAPIFromDeviceConfig(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document portDisplayConfigDoc, Element portElement) throws Exception;
	public abstract Document generateWindowAPIFromDeviceStatus(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document portDisplayStatusDoc, Element portElement) throws Exception;
	public abstract Document generateDeviceAgentRequestFromWindowAPI(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element setPortDisplayConfigElement, Element portElement) throws Exception;

	public abstract Element getDisplayStatusElement(Element sourceElement,MRLog log) throws Exception;
	public abstract Element getDisplayConfigElement(Element sourceElement,MRLog log) throws Exception;
	
	public abstract void updateConfigInDatabase(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document  deviceConfigurationRequestDoc, Element portElement) throws Exception;

	
	
	protected void updateInDatabase(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Document  deviceConfigurationRequestDoc, Element portElement,String portObjectType,String updateChildXpath)
	throws Exception {
		Document deviceDoc = PortBaseRequestHandler.getDeviceDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, portElement, updateChildXpath,
				portObjectType, portObjectType);
		String deviceNID = MRXMLUtils.getAttributeValueXpath(deviceDoc,"//"+SessionServiceConst.DEVICE,SessionServiceConst.NID,"");
		Element childEl  = MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, updateChildXpath);
		AssetAdminServiceUtils.updateChild(deviceNID, updateChildXpath, (Element)childEl.clone(), request, log, db, xmpp, resourceFactory, serviceAgentFactory);
	}
	
}
