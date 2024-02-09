package com.ipvs.common.db;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.service.MRXMLUtils;
public class ServiceRequest {
	private String handlerName = null;
	public static final String HANDLER_ROOT = "Request";
	private ServiceRequestAction action=null;
	private ServiceRequestHeader header=null;
	private String dbObjectName=null;
	private String dataNodeName= null;//"AssetManagementRequestData";

	public ServiceRequest(){
		this.handlerName = HANDLER_ROOT;
	}
	public Element getHeaderEL() throws Exception {
 		return (Element)getHeader().getHeaderEL().clone();
	}
	public Element getActionEL() throws Exception {
 		return (Element)getAction().getActionEL().clone();
	}
	public String getHandlerName() {
		return handlerName;
	}
	public ServiceRequestAction getAction() {
		if(action==null)
			action= new ServiceRequestAction();
		return action;
	}
	public ServiceRequestHeader getHeader() {
		if(header==null)
			header= new ServiceRequestHeader();
		return header;
	}
	public void setHandlerName(String handlerName) {
		this.handlerName = handlerName;
	}
	public void setAction(ServiceRequestAction action) {
		this.action = action;
	}
	
	public void setHeader(ServiceRequestHeader header) {
		this.header = header;
	}
	public String getDbObjectName() {
		return dbObjectName;
	}
	public void setDbObjectName(String dbObjectName) {
		this.dbObjectName = dbObjectName;
	}
	public Document getDoc() throws Exception {
		Document doc = null;
		doc = MRXMLUtils.getDocument();
		Element root= MRXMLUtils.addElement(doc, handlerName);
		MRXMLUtils.addAttribute(root, "serviceVer", MRXMLUtils.getServiceVersion(null));
		MRXMLUtils.addElement(doc.getRootElement(), (Element)header.getHeaderEL().clone());
		Element dataEL = MRXMLUtils.addElement(doc.getRootElement(), "Data");
		dataEL = MRXMLUtils.addElement(dataEL, dataNodeName);
		if(!action.getAction().equals(ServiceRequestAction.ACTION_QUERY_REQUEST)){
			MRXMLUtils.addElement(dataEL, "ObjectType",dbObjectName);
		}
		MRXMLUtils.addElement(dataEL, (Element)action.getActionEL().clone());
		return doc;
	}
	public Document  saveDoc(String filePath) throws Exception {
		Document doc = getDoc();
		MRXMLUtils.saveDocument(filePath, doc);
		return doc;
	}
	public String getDataNodeName() {
		return dataNodeName;
	}
	public void setDataNodeName(String dataNodeName) {
		this.dataNodeName = dataNodeName;
	}
}

