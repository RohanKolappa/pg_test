package com.ipvs.common.service;
/*
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Text;
import org.xml.sax.InputSource;
*/
public class MRService  {
	
	protected String serviceName;
	protected String handlerPackage;
	protected String defaultHandlerClass;

	public MRService(String serviceName) {
		this.serviceName = serviceName;
		this.handlerPackage = "com.ipvs." + serviceName + ".handler";
		this.defaultHandlerClass = handlerPackage + ".DefaultRequestHandler";
	}
		
	public String getName() {
		return serviceName;
	}
	
	
	public MRRequestHandlerI getHandler(String requestName) throws Exception {
		String handlerName = handlerPackage + "." + requestName + "Handler";
		try {
			Class<?> c = Class.forName(handlerName);
			return (MRRequestHandlerI)c.newInstance();
		}
		catch(Exception e) {
			Class<?> c = Class.forName(defaultHandlerClass);
			return (MRRequestHandlerI)c.newInstance();
		}
	}

}
