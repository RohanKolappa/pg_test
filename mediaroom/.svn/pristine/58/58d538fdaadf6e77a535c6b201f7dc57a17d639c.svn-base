package com.ipvs.common.db;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.customvalidation.ValidatorPluginDelegate;
import com.ipvs.customvalidation.ValidatorPluginDelegateFactory;
import com.ipvs.utils.Utils;

public class CustomValidation {
	private Document doc = null;

	public CustomValidation() {

	}

	public Document getDoc() {
		return doc;
	}

	public void setDoc(Document doc) {
		this.doc = doc;
	}

	public boolean customValidateActionReq(String action, String serviceName,  String dbObjectName, ValidatorPluginDelegate defaultValidator, Element parentEL, Element validateParentEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		boolean bSuc =false;
		bSuc = Utils.booleanValue(MRXMLUtils.getAttributeValue(validateParentEL, "enable", "false"),false);
		if(bSuc){
			if (defaultValidator == null) {
				String defaultValidatorClass = MRXMLUtils.getAttributeValue(validateParentEL, "defaultValidatorClass", "");
				defaultValidator = ValidatorPluginDelegateFactory.getInstance().getValidatorPluginDelegate(defaultValidatorClass);
			}
			String xpath = "//validation[@action='"+action+"']";
			List<?> childList = MRXMLUtils.getListXpath(validateParentEL, xpath);
			for (Iterator<?> i = childList.iterator(); i.hasNext();) {
				Element childValidatedEL = (Element) i.next();
		        boolean enable = Utils.booleanValue(MRXMLUtils.getAttributeValue(childValidatedEL, "enable", "false"),false);
				if(!enable){
				    continue;
				}
				String methodName = MRXMLUtils.getAttributeValue(childValidatedEL, "methodName", "").trim();
				String validatorClass = MRXMLUtils.getAttributeValue(childValidatedEL, "validatorClass", "").trim();
				String childXpath = MRXMLUtils.getAttributeValue(childValidatedEL, "xpath", "").trim();
				boolean overWrite = Utils.booleanValue(MRXMLUtils.getAttributeValue(childValidatedEL, "overWrite", "false"),false);
				
				Element childReqEL = parentEL;
				if( !childXpath.trim().equals("") ) {
					childReqEL = MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(parentEL), childXpath);
					if(childReqEL == null){
						continue;
					}
				} 
				if(childReqEL == null){
					continue;
				}			
				//Element childReqEL = MRXMLUtils.getElementXpath(parentEL.getDocument(), childXpath);
				//if(childReqEL != null){
				if (!methodName.equals("")) {
					if (!validatorClass.equals("")) {
						ValidatorPluginDelegate validator = ValidatorPluginDelegateFactory.getInstance().getValidatorPluginDelegate(validatorClass);
						bSuc = validator.validateEL(action, serviceName, dbObjectName, childReqEL, childValidatedEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
						if (!bSuc) {
						// TODO: raise error
							return bSuc;
						}
					} else {
						bSuc = defaultValidator.validateEL(action, serviceName, dbObjectName, childReqEL, childValidatedEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
						if (!bSuc) {
						// TODO: raise error
							return bSuc;
						}
					}
					if(overWrite) {
						MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(parentEL, childXpath), childReqEL);
					}
				}
				
				//}
			}
		}else{
			bSuc = true;
		}
		return bSuc;
		
	}
	
/*
	private boolean customValidateEL(String action, String serviceName,  String dbObjectName, String path, ValidatorPluginDelegate defaultValidator, Element parentEL, Element validateParentEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		boolean bSuc =false;
		List<?> childList = MRXMLUtils.getList(validateParentEL);
		for (Iterator<?> i = childList.iterator(); i.hasNext();) {
			Element chilValidatedEL = (Element) i.next();
			String childNodeName = chilValidatedEL.getName();
			String methodName = MRXMLUtils.getAttributeValue(chilValidatedEL, "methodName", "").trim();
			String validatorClass = MRXMLUtils.getAttributeValue(chilValidatedEL, "validatorClass", "").trim();
			String childXpath = path + "/" + childNodeName;
			Element childReqEL = MRXMLUtils.getElementXpath(parentEL.getDocument(), childXpath);
			if (!methodName.equals("")) {
				if (!validatorClass.equals("")) {
					ValidatorPluginDelegate validator = ValidatorPluginDelegateFactory.getInstance().getValidatorPluginDelegate(validatorClass);
					bSuc = validator.validateEL(action, serviceName, dbObjectName, childReqEL, chilValidatedEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
					if (!bSuc) {
						// TODO: raise error
						return bSuc;
					}
				} else {
					bSuc = defaultValidator.validateEL(action, serviceName, dbObjectName, childReqEL, chilValidatedEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
					if (!bSuc) {
						// TODO: raise error
						return bSuc;
					}
				}
			}
			bSuc =  customValidateEL(action,  serviceName, dbObjectName, childXpath, defaultValidator, childReqEL, chilValidatedEL, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
			if (!bSuc) {
				// TODO: raise error
				return bSuc;
			}
		}
		return true;
	}*/
	
 
}
