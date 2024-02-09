package com.ipvs.assetadminservice.device.handler;

import java.net.URLDecoder;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Node;


import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.port.handler.SetPortResourceTypeRequestHandler;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.AutoStartManager.Action;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.AddUserRequestHandler;
import com.ipvs.sessionservice.resource.handler.SetUserPasswordRequestHandler;
import com.ipvs.sessionservice.window.handler.DisplayUtils;
import com.ipvs.sessionservice.window.handler.SessionServiceWindowConst;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public abstract class BaseSetServiceClientRequestHandler extends SetPortBaseRequestHandler {
	
	private static final String PORT_TYPE_CHANGE_XPATH = "//XP200PortConfig/Info"; 
	private static Logger logger = Logger.getLogger(BaseSetServiceClientRequestHandler.class);
	private static final String PORT_STATE_CHANGED_XPATH = "//AVCodecElementsStatus/*/.[PortID="; 
	public static final String ADMIN_SERVICE_CONFIG_CHANGE_XPATH = "//AdminServiceConfig";
	private enum PortStateEnum { Active,Inactive;
	public static String getPortStatus(String portState) {
		switch(PortStateEnum.valueOf(portState)){

		case Active: return AMConst.ASSETSTATE_READY; 
		case Inactive: return AMConst.ASSETSTATE_OFFLINE; 
		default: return "";
		}
	}
	public static String getPortPresence(String portState) {
		switch(PortStateEnum.valueOf(portState)){
		case Active: return AMConst.AVAILABLE;
		case Inactive:return AMConst.UNAVAILABLE;
		default: return "";
		}
	}
	
	public static String getOwnerJID(String portState,MRRequest request) {
		switch(PortStateEnum.valueOf(portState)){
		case Active: return request.getUserJID();
		case Inactive:return "";
		default: return "";
		}
	}
	
}
	
	
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);
		return auth;
	}
	
	
	private Element handleconfigUpdate(Document notificationDoc,MRRequest request,MRDB db,MRLog log) throws MRException {
		Element el = null;

		Document dochandler = null;
		Document docbasecfg = null;	
		
		String deviceName="V2D-XPi-Device";

		try {
			el = MRXMLUtils.getElementXpath(notificationDoc, "//"+"SetServiceClientConfig");
			String byNID = MRXMLUtils.getAttributeValueXpath(notificationDoc, "//"+"SetServiceClientConfig", "byNID", "");
			//System.out.println("Command Status:="+el.asXML());

			
			String cmd = MRXMLUtils.getValueXpath(el, ".//CLICommand", null);
			
			String status = MRXMLUtils.getValueXpath(el, "./"+"CLIConfig"+"/Command/Status", "Failure");
		
			if (status.compareToIgnoreCase("Success") != 0) {
				logger.info("NGSD200AdminPlugin.handleconfigUpdate Ignoring ERROR " + cmd);
				return null;
			}

			if ( cmd.equalsIgnoreCase("set_xpi_identifier")) {		//set_xpi_identifier handled as status update, ignore in configupdate	
				return null;
			}

			dochandler = MRXMLUtils.loadXMLResource("/com/ipvs/xsd/xml/confighandler.xml",log);
			/*String query = "{'@NID':'"+byNID+"'}"; //TBD: NID
			String deviceStr =mongodb.getDocument("Device", query);
			deviceStr = (!deviceStr.isEmpty())  ? "{Device: "+deviceStr+"}": deviceStr;*/
			
			docbasecfg=MRXMLDBUtils.readElement(request, log, db, byNID);
			
			String doDBUpdate=MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/dbupdate", "true");			
			String updateXPath = MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/triggername", null);
			String action = MRXMLUtils.getValueXpath(dochandler,
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/action", null);
			String updateParentXPath=null;
			String operation = AMConst.UPDATE;
			if (action.compareToIgnoreCase(AMConst.ADD)==0) {
				updateParentXPath = MRXMLUtils.getValueXpath(dochandler, 
						"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/tablename", null);
				operation = AMConst.ADD;
			} if(action.compareToIgnoreCase("Delete")==0){
				operation = AMConst.DELETE;
			}


			if (updateXPath == null) {
				logger.info("NGSD200AdminPlugin.handleconfigUpdate Ignoring Command " + cmd);
				return null;
			}
			try {		
				String locatorName=null;
				String locatorRef=null;
				String tableLocatorRef=null;
				String locatorValue=null;
				String indexValue=null;
				Element eh=null;
				eh = MRXMLUtils.getElementXpath(dochandler, 
						"//handler[cmd='" + cmd + "' and device='" + deviceName +"']");					
				Element parentEl = eh.element("parent");
				if(parentEl != null) {
					locatorRef = MRXMLUtils.getAttributeValueWithAlternate(parentEl, "relativeXpath","", null);				
					if (locatorRef != null) {
						locatorName = MRXMLUtils.getAttributeValueWithAlternate(parentEl, "name","", null);
						locatorValue = MRXMLUtils.getValueXpath(el, "//"+ locatorName, null);
						updateXPath += "[" + locatorRef +"=" + locatorValue + "]";
						if (updateParentXPath != null) {
							tableLocatorRef = MRXMLUtils.getAttributeValueWithAlternate(parentEl, "tablerelativeXpath","", null);
							updateParentXPath += "[" + tableLocatorRef +"='" + locatorValue + "']";
						}
					}
				}
				indexValue = MRXMLUtils.getValueXpath(el, "//Command/Index", null);
				if (indexValue != null) {
					updateXPath += "[@ListIndex=" + indexValue + "]";
				}
			} catch (Exception e) {
			
			}

			//set flag to dirty=clean if empty
			updateDirtyChildrenToClean(docbasecfg.getRootElement(),"");

			if (el != null) {
				updateConfig(el, dochandler, docbasecfg, cmd,log);				

				if (doDBUpdate.compareToIgnoreCase(AMConst.TRUE)==0) {
					Element deviceConfigEl = (Element)el.clone();
					log.addInfo("deviceConfigEl::::::::::::::::::"+deviceConfigEl.asXML());
					Element config = MRXMLUtils.getElementXpath(deviceConfigEl, ".//"+"CLIConfig");
					log.addInfo("Element::::::::::::"+((Element)config.detach()).asXML());
					log.addInfo("deviceConfigEl1111::::::::::::::::::"+deviceConfigEl.asXML());
					String xpath = updateXPath;
					if(operation.equals(AMConst.ADD)) {
						xpath = updateParentXPath;
					}					
					deviceConfigEl.addAttribute(AMConst.OPERATION_ATTRIBUTE, operation);
					deviceConfigEl.addAttribute(AMConst.XPATH_ATTRIBUTE, xpath);
					if(!operation.equals(AMConst.DELETE)) {
						deviceConfigEl.add((Element)DocumentHelper.createXPath(updateXPath).selectSingleNode(docbasecfg).clone());
					}
					updateDirtyChildrenToClean(deviceConfigEl, action);
					return deviceConfigEl;

				}		
				else
					return null;
			}
			else return null;
		} catch (Exception e) {
			e.printStackTrace();
			logger.error("Error while applying configupdate: "+e.getStackTrace());
			throw new MRException(1039, "Error while applying config");
		}
	}
	
	
	public synchronized Element updateConfig(Element el, Document dochandler, Document docbasecfg, String cmd,MRLog log) {		
		Element xmlsnippet = null;
		Element child = null;

		String action = null;
		String deviceName = null;
		Element deviceConfig = null;

		try {
			deviceConfig = docbasecfg.getRootElement();
		} catch (Exception e) {
			e.printStackTrace();
		}


		if (deviceConfig != null) {
			deviceName = MRXMLUtils.getAttributeValue(deviceConfig, "type");
		}

		if (cmd == null) {
			return null;
		}

		xmlsnippet = DocumentHelper.createElement(cmd);

		Iterator<?> params = el.element("CLIConfig").element("Parameters").elements().iterator();
		Element elparams;

		while (params.hasNext()) {
			elparams = (Element) params.next();
			child = DocumentHelper.createElement(elparams.elementText("Name"));
			child.addAttribute("value", elparams.elementText("Value"));

			try {
				xmlsnippet.add(child);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}


		Element apply = null;
		Element cfgElement = null;
		String trigger = null;
		String table=null;
		String xpath = null;
		boolean matched = false;
		boolean found = false;
		String type = null;
		String locatorXpath=null;
		String defaultFile="";
		String indexattr = "";

		try {
			apply = MRXMLUtils.getElementXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + "V2D-XPi-Device" +"']");

			action = apply.element("action").getStringValue();

			logger.info("updateConfig :" + cmd + "->" + action);


			found = true;
			matched= true;

			trigger = apply.elementText("triggername");
			try {
				table = apply.elementText("tablename");
				defaultFile = apply.elementText("default");
			}
			catch (Exception e) {
			}	

			Iterator<?> applyargs = apply.element("paramtable").elements().iterator();
			String name = null;
			String value = null;
			locatorXpath=trigger;

			if ((action.compareToIgnoreCase("updatelist") == 0) || 
					(action.compareToIgnoreCase("delete") == 0) ) {
				indexattr = "[@ListIndex=\"" + el.element("CLIConfig").element("Command").element("Index").getStringValue() + "\"]";
				logger.info("updateConfig :" + action + indexattr);
			}
			// Get Locator Lists
			String locatorName=null;
			String locatorRef=null;
			if (action.compareToIgnoreCase("add") == 0) {
				locatorXpath=table;
				try {
					locatorName=apply.element("parent").attributeValue("name");
					locatorRef=apply.element("parent").attributeValue("tablerelativeXpath");
					String locatorValue= el.element("CLIConfig").element("Command").element(locatorName).getStringValue();
					locatorXpath += "[" + locatorRef +"=\"" 
							+ locatorValue + "\"]";
				}
				catch (Exception e) {
					e.printStackTrace();
				}
			}
			else {
				locatorXpath=trigger;
				try {
					locatorName=apply.element("parent").attributeValue("name");
					locatorRef=apply.element("parent").attributeValue("relativeXpath");
					String locatorValue= el.element("CLIConfig").element("Command").element(locatorName).getStringValue();
					locatorXpath += "[" + locatorRef +"=\"" 
							+ locatorValue + "\"]";
				}
				catch (Exception e) {
				}
			}

			while (applyargs.hasNext()) {
				child = (Element) applyargs.next();

				type = child.attributeValue("type");

				if (type == null) {
					type = "String";
				}

				name = child.elementText("paramname");
				if (action.compareToIgnoreCase("add") == 0) {
					xpath = trigger + "/" + child.elementText("paramval");
				}
				else {
					xpath = locatorXpath + indexattr + "/" + child.elementText("paramval");
				}
				child = xmlsnippet.element(name);

				if (child != null) {
					child.addAttribute("xpath", xpath);

					// Fix Booleans -- map Yes/No to true/false
					if (type.compareTo("Boolean") == 0) {
						value = child.attributeValue("value");

						if (value.compareToIgnoreCase("yes") == 0) {
							child.addAttribute("value", "true");
						} else if (value.compareToIgnoreCase("no") == 0) {
							child.addAttribute("value", "false");
						}
					}
					else if (type.compareTo("URLString") == 0) {
						value = child.attributeValue("value");
						child.addAttribute("value", 
								URLDecoder.decode(value, "UTF-8"));
					}
				} else {
					logger.info("updateConfig :Warning: Name not found " + cmd + ":" + name);
				}
			}

			applyargs = xmlsnippet.elements().iterator();

			while (applyargs.hasNext()) {
				child = (Element) applyargs.next();

				if (child.attributeValue("xpath") == null) {
					logger.info("updateConfig :Warning Ignoring " + cmd + ":" + child.getName());
				}
			}
		}
		catch (Exception e) {
			e.printStackTrace();
		}

		if (matched) {
			Document doc = null;
			Element c1 = null;

			if (action.compareToIgnoreCase("add") == 0) {
				// Get the element from the object Xml		
			
				String index = el.element("CLIConfig").element("Command").element("Index").getStringValue();
				try {
					doc=MRXMLUtils.loadXMLResource(defaultFile,log);
					c1 = MRXMLUtils.getElementXpath(deviceConfig, "."+locatorXpath);//(Element) DocumentHelper.createXPath(locatorXpath).selectSingleNode(deviceConfig);	
					Element childEl = MRXMLUtils.getElementXpath(c1, "."+trigger+"[@ListIndex='"+index+"']");
					if(childEl!=null)
						childEl.detach();
				} catch (Exception e) {
					e.printStackTrace();
				}
				cfgElement = doc.getRootElement();
				cfgElement.addAttribute("ListIndex", index);
			} else if (action.compareToIgnoreCase("delete") == 0) {
				deleteEntry(deviceConfig, locatorXpath + indexattr);
			} else {
				cfgElement = deviceConfig;
			}
			if (cfgElement != null) {
				List<?> listChild = xmlsnippet.elements();
				for (Object object : listChild) {
					setXpath_decendents((Element)object, cfgElement, cmd);
				}
				if (action.compareToIgnoreCase("add") == 0) {
					cfgElement.detach();	
					
					c1.add(cfgElement);
				}
			}

			logger.info("updateConfig :Updated Command " + cmd);
		} else {
			if (found == false) {
				logger.info("updateConfig :Error Could not handle :" + cmd);
			}
		}

		return deviceConfig;
	}
	
	
	private void setXpath_decendents(Element child, Element cfgElement, String cmd){
		String name = null;
		String value = null;

		name = child.attributeValue("xpath");

		if (name == null) {
			return;
		}

		value = child.attributeValue("value");

		try {
			Node element = DocumentHelper.createXPath(name).selectSingleNode(cfgElement);
			if(element!=null)
			{
				element.setText(value);
				//			if (MRXMLUtils.setValueXpath(cfgElement, name, value) == null) {
				//				System.err.println("Error: Failed to update " + name + " for " + cmd);
				//			}
				String ele=element.asXML();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		List<?> listChild = child.elements();
		for (Object object : listChild) {
			setXpath_decendents((Element)object, cfgElement, cmd);
		}
	}
	
	
	private void deleteEntry(Element deviceConfig, String trigger) {
		List<?> l = null;
		Element el = null;

		try {
			l = MRXMLUtils.selectNodesXpath(deviceConfig, trigger);
		} catch (Exception e) {
			e.printStackTrace();
		}

		if ((l != null) && (!l.isEmpty())) {
			for (int i = 0; i < l.size(); i++) {
				el = (Element) l.get(i);
				el.detach();
			}
		}
	}	
	
	
	public void updateDirtyChildrenToClean(Element updatedoc, String attributeVal){
		try{
			String xPath = "";
			List<?> dirtyList = null;
			xPath = ".//*[@" + AMConst.DIRTY_TAG + "]";
			dirtyList = DocumentHelper.createXPath(xPath).selectNodes(updatedoc);
			for (Object name : dirtyList) {
				Element dirtyEL = (Element) name;
				String attrb = MRXMLUtils.getAttributeValueWithAlternate(dirtyEL, AMConst.DIRTY_TAG,"",null);
				if (attrb != null &&   attrb.trim().equalsIgnoreCase(attributeVal)  ) {
					MRXMLUtils.addAttribute(dirtyEL, AMConst.DIRTY_TAG,AMConst.CLEAN_FLAG);
				} 
			}
		}catch(Exception ex){
			logger.error("Error in updating dirty flags: ", ex);
		}
	}
	
	
	public Element translateDeviceConfigRequest(Document requestDocument,MRRequest request,MRDB db,MRLog log) throws Exception {		
		Element deviceConfig = handleconfigUpdate(requestDocument,request,db,log);	
		/*if(deviceConfig!=null){
			MRXMLUtils.getElementXpath(requestDocument, "//"+"SetServiceClientConfig").detach();		
			MRXMLUtils.addElementXpath(requestDocument, "//"+"SetServiceClientConfig"+"RequestData", deviceConfig);
			return requestDocument;
		}*/
		return deviceConfig;
			
	}
	
	
	
	
	public String matchErrorState(String type, String cmd, Document dochandler, Document notificationDoc) throws Exception{
		List<?> errorList = DocumentHelper.createXPath("//handler[cmd='" + cmd + "']/error[@type='" + type + "']").selectNodes(dochandler);
		if((errorList != null) && !errorList.isEmpty()) {
			for(Object error:errorList) {
				Element errorEl = (Element)error;
				String errorName = MRXMLUtils.getAttributeValueWithAlternate(errorEl, "name","", "");
				String errorValue = MRXMLUtils.getAttributeValueWithAlternate(errorEl, "value","", "");
				String errorType = MRXMLUtils.getAttributeValueWithAlternate(errorEl, "type","", "");
				String xpath = "//Parameter[@name='" + errorName + "' and .='" + errorValue + "']";
				List<?> l =  DocumentHelper.createXPath(xpath).selectNodes(notificationDoc);
				if(l != null && !l.isEmpty()) {
					return errorType;
				}
			}
		}
		return null;
	}
	
	


	
	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		RequestProcessor requestProcessor  = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory , AMConst.Device);
		String type = MRXMLUtils.getAttributeValue(requestProcessor.getRequestElement().getParent(), MRConst.TYPE,"");
		if (type.equals("V2D-XPi-Device"))
		{
		Element deviceConfigElement=translateDeviceConfigRequest(MRXMLUtils.elementToDocument(requestProcessor.getRequestElement()),request,db,log); 
		requestProcessor.setRequestElement(deviceConfigElement);
		}
		
		Element dataEl=MRXMLUtils.getFirstChild(requestProcessor.getRequestElement());
		String operation = MRXMLUtils.getAttributeValue(requestProcessor.getRequestElement(), MRConst.OPERATION_ATTRIBUTE,"");
		log.addInfo("operation:::::::::::::::::::::::"+operation);
		String xpath = MRXMLUtils.getAttributeValue(requestProcessor.getRequestElement(), MRConst.XPATH_ATTRIBUTE,"");
		log.addInfo("xpath:::::::::::::::::::::::"+xpath);
		String thumbnailSupport = MRXMLUtils.getAttributeValue(requestProcessor.getRequestElement(), "thumbnail","false");
		log.addInfo("thumbnailSupport:::::::::::::::::::::::"+thumbnailSupport);
		log.addInfo("NID:::::::::::::::::::::::"+requestProcessor.getNID());
		if(dataEl ==null && MRConst.OPERATION_ENUM.valueOf(operation) != MRConst.OPERATION_ENUM.Delete && MRConst.OPERATION_ENUM.valueOf(operation) != MRConst.OPERATION_ENUM.UpdateAttribute) {
			throw new MRException(AMExceptionCodes.INVALID_DATA, "Invalid Data");
		}
		Semaphore lock = IPVSMonitor.getMonitor().getLock(requestProcessor.getNID(), request.getNID());		
		log.addInfo("BaseServiceClientRequestHandler: Aquire lock on deviceNID:="+requestProcessor.getNID(), MRLog.INFO, MRLog.NOTIFY);
		Exception exp = null;					
	
		try {
			switch(MRConst.OPERATION_ENUM.valueOf(operation)) {
				case Add:
					MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, request, log, db, 
							"", requestProcessor.getObjectType(), requestProcessor.getNID(), xpath, null, dataEl);
					break;

				case Update:
					//check if the request is for display status update
					if(xpath.startsWith(SessionServiceWindowConst.DISPLAY_STATUS_XPATH) || xpath.startsWith(SessionServiceWindowConst.DISPLAY_CONFIG_XPATH)){
						String notificationType = SessionServiceWindowConst.DISPLAY_CONFIG;
						if(xpath.startsWith(SessionServiceWindowConst.DISPLAY_STATUS_XPATH)) {
							notificationType = SessionServiceWindowConst.DISPLAY_STATUS;
						}
						DisplayUtils.queueDisplayNotification(request, xmpp, log, db,serviceAgentFactory, xpath, requestProcessor.getNID(),dataEl, null,null,notificationType,false);
					}
					MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, 
							"", requestProcessor.getObjectType(), requestProcessor.getNID(), xpath, null, dataEl);
					log.addInfo("BaseServiceClientRequestHandler: Debug log added by padmaja db update done"+requestProcessor.getNID(), MRLog.INFO, MRLog.NOTIFY);
					
					if(xpath.startsWith(PORT_TYPE_CHANGE_XPATH)) {
						updatePorType(request, xmpp, log, db,serviceAgentFactory,resourceFactory, requestProcessor.getNID(),dataEl,xpath,thumbnailSupport);
					}
					
					if(xpath.startsWith(ADMIN_SERVICE_CONFIG_CHANGE_XPATH) || xpath.startsWith("//DeviceConfig")) {
						
						//TBR: Device config xpath should be removed. this is older ms device app support						
						String xmppServerEnable = "";
						if(xpath.startsWith("//DeviceConfig")){
							xmppServerEnable = MRXMLUtils.getValueXpath(dataEl, ".//XMPPServer/XMPPServerEnable", "");
						}
						
						if(xmppServerEnable.isEmpty()) {
							xmppServerEnable = MRXMLUtils.getValueXpath((AssetAdminServiceUtils.getDeviceDoc(requestProcessor.getNID(), db, log, request.getUserJID())), "//XMPPServer/XMPPServerEnable", "");
						}
						
						Boolean msFlag = Utils.getBoolean(xmppServerEnable, false);
						if(msFlag) {
							updateAdminPassword(request, xmpp, log, db,serviceAgentFactory,resourceFactory, requestProcessor.getNID(), dataEl, xpath);
						}
					}
					/*
					 * Check if standby server exists and ready
					 * if yes, send active config update request
					 */
					/*MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.SERVICE_RESOURCE, log);
					String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
					String whereXPath[] = {"//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE+ "']"};
					Element serviceResourceElement = MRXMLDBUtils.readElement(1, null, whereXPath, rootNID, 1, null, null, request, db, log);*/
					Element serviceResourceElement = HAUtils.getServiceResourceElement(db, request, log);
					if(serviceResourceElement!=null && !xpath.startsWith("//DeviceConfig")) {
					    String standbyAssetResourceNID = HAUtils.getMsAssetResourceNID("standby", serviceResourceElement, log);		    
					    if(!standbyAssetResourceNID.isEmpty())		    
						HAUtils.sendPrimaryConfigUpdateEvent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, HAUtils.getDeviceJIDFromAssetResourceNID(request, log, db, standbyAssetResourceNID), request.getUserJID(), standbyAssetResourceNID, serviceResourceElement, this.getClass().getName(), dataEl,xpath);
					}

					break;
					
					
				case UpdateAttribute:
					
				 	String data= MRXMLUtils.getValue(requestProcessor.getRequestElement());
				 	if(data.isEmpty()) {
				 		throw new MRException(AMExceptionCodes.INVALID_DATA, "Invalid Data");
				 	}
				 	
			        MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, "", SessionServiceConst.DEVICE, requestProcessor.getNID(), xpath, null, data);
			        //check if the port status changes then update port status 
					if(xpath.startsWith(PORT_STATE_CHANGED_XPATH)) {
						updatePortState(request,xmpp,log,db,serviceAgentFactory,resourceFactory,requestProcessor.getNID(),data,xpath);						
					}
			        break;
					
				case Delete:

					MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, 
							request, log, db, "", requestProcessor.getObjectType(), requestProcessor.getNID(), xpath, null);
					break;
			}
		} catch (Exception e) {
			exp =e;
		}
		
		log.addInfo("BaseServiceClientRequestHandler: "+ operation +" Device Config/Status. NodeXpath:="+xpath, MRLog.INFO, MRLog.NOTIFY);
		IPVSMonitor.getMonitor().releaseLock(requestProcessor.getNID(), lock);
		log.addInfo("BaseServiceClientRequestHandler: Release lock on deviceNID:="+requestProcessor.getNID(), MRLog.INFO, MRLog.NOTIFY);
		
		if(exp !=null) { 
			throw exp;
		}
		
	  	return request.createResponseData();
	}


	private void updatePortState(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory,
			MRResourceFactoryI resourceFactory, String nid, String data, String xpath) throws MRException, Exception {
		String portID ="";
		String regex = "PortID\\s*=\\s*(.*?)]";
		Matcher matcher = Pattern.compile(regex).matcher(xpath);
        if(!matcher.find()) {
			return;
		}
        portID = matcher.group(1);
        if(portID == null || portID.isEmpty()) {
			return;
		}
		
		Element assetResourceNIDEl = AssetAdminServiceUtils.getChild(nid, "//AssetResourceNID", request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		String assetResourceNID = MRXMLUtils.getValueXpath(assetResourceNIDEl,".//AssetResourceNID","");
		
		String portState = StringUtils.substringBetween(data, "{\"", "\"}");
				
		String portResourceStatus = PortStateEnum.getPortStatus(portState);
		String portResourcePresence = PortStateEnum.getPortPresence(portState);
		String ownerJID = PortStateEnum.getOwnerJID(portState, request);
			
		String query = FileUtils.readFileResource("/com/ipvs/assetadminservice/handler/xquery/UpdatePortPresenceQuery.xml", request.getClass());
		query = StringUtils.replace(query, "_ASSETRESOURCENID_", assetResourceNID);
		query = StringUtils.replace(query, "_PORTID_", portID);
		query = StringUtils.replace(query, "_OWNERJID_", ownerJID);
		query = StringUtils.replace(query, "_PRESENCE_", portResourcePresence);
		query = StringUtils.replace(query, "_STATE_", portResourceStatus);
		String response = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);		
		log.addInfo("Update Port State :="+portResourceStatus ,MRLog.INFO,MRLog.NOTIFY);		
		
		
		//generate port notification
		Element portInfo = MRXMLUtils.stringToElement(response);
		String portNID = MRXMLUtils.getAttributeValue(portInfo, SessionServiceConst.NID,"");
		Element portElement =  MRXMLUtils.addElement("SetPort");
		portElement.addAttribute(SessionServiceConst.WHERE_NID, portNID);
		MRRequest newRequest  = MRRequest.createRequest(request,MRXMLUtils.elementToDocument(portElement),"SetPortRequest","","");
		RequestProcessor requestObject = new RequestProcessor(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.PORT);
		super.generatePortUpdateNotifications(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		
		//autostart stream for sourcePorts
		String streamType = MRXMLUtils.getElementXpath(portInfo, ".//Info/Type").getText();

		if(streamType.equals("StreamSrc")){
			Element ethernetPortConfigEL = MRXMLUtils.getElementXpath(portInfo, "//EthernetPortStatus/Parameters[Parameter[@name='Status']='True' and Parameter[@name='Primary']='True']/Parameter[@name='IPAddress']");
			String ipAddress ="";
			if(ethernetPortConfigEL!=null) {
				//System.out.println("In get IP Address based on url scheme " + MRXMLUtils.elementToString(ethernetPortConfigEL));
				ipAddress = ethernetPortConfigEL.getText();
			}
			log.addInfo("BaseServiceClientRequestHandler:updatePortState + autostart ipaddress="+ipAddress);
			String enableAutoStart = MRXMLUtils.getAttributeValueXpath(portInfo, ".//Instance", "mode", SessionServiceConst.MODE_DYNAMIC);
			if(enableAutoStart.equalsIgnoreCase(SessionServiceConst.MODE_AUTO_START)){
				request.setSuperUser(true);
				AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.CONTINUE_AUTOSTART_SESSION,ipAddress);			
			}else{
				request.setSuperUser(true);
				AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.STOP_AUTOSTART_SESSION,"");
			}			
		}
		
		
	}


	private void updatePorType(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String nid,Element dataEl,String xpath,String thumbnailSupport) throws MRException, Exception {
	
		String portID ="";
		String regex = "../PortID\\s*=\\s*(.*?)]";
		Matcher matcher = Pattern.compile(regex).matcher(xpath);
        if(!matcher.find()) {
			return;
		}
        portID = matcher.group(1);
		if(portID == null || portID.isEmpty()) {
			return;
		}
		
		Element assetResourceNIDEl = AssetAdminServiceUtils.getChild(nid, "//AssetResourceNID", request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		String assetResourceNID = MRXMLUtils.getValueXpath(assetResourceNIDEl,".//AssetResourceNID","");	
		Document portTypeChangeRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/assetadminservice/port/xml/SetPortResourceTypeRequest.xml", log);
		Element requestElement = MRXMLUtils.getElementXpath(portTypeChangeRequestDoc, "//SetPortResourceType");
		MRXMLUtils.setAttributeValue(requestElement, AMConst.BY_ASSET_NID_ATTRIBUTE, assetResourceNID);
		Element portElement = MRXMLUtils.getElement(requestElement, "PortResource");
		MRXMLUtils.setAttributeValue(portElement,AMConst.DEVICE_ELEMENT_ID_ATTRIBUTE, portID);
		String portType = MRXMLUtils.getValueXpath(dataEl, ".//PortIOType", "");
		MRXMLUtils.setAttributeValue(portElement, AMConst.TYPE_ATTRIBUTE,portType);
        MRXMLUtils.setAttributeValue(portElement, "thumbnail",thumbnailSupport );
		MRRequest portTypeChangeRequest  = MRRequest.createRequest(request.getUserJID(), portTypeChangeRequestDoc, request.getAppServerJID());		
		new SetPortResourceTypeRequestHandler().getResponse(portTypeChangeRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);		
	}
	
	
	public void updateAdminPassword(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, ServiceAgentFactoryI serviceAgentFactory, MRResourceFactoryI resourceFactory, String nid,Element dataEl,String xpath) throws MRException, Exception {
		
		MRConfig mrConfig = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, PolicyAdminConstants.USER, log);
        String parentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));	
        String[] whereXPath = {"//" + PolicyAdminConstants.USER + "[" + PolicyAdminConstants.USER_NAME + "='" + AMConst.ADMIN + "']"};
        
    	Element userEL = MRXMLDBUtils.readElement(0, null, whereXPath, parentNID, null, request, log, db);
    	String username = MRXMLUtils.getValueXpath(userEL, "//Username", "");
    	String oldPassword = MRXMLUtils.getValueXpath(userEL, ".//Password", "");
    	String newPassword = MRXMLUtils.getValueXpath(dataEl, ".//Password", "");
        if (userEL == null) {
        	new AddUserRequestHandler().addUserInDB(request, AMConst.ADMIN, AMConst.ADMIN, newPassword, "", new Date().toString(), new Date().toString(), null, xmpp, log, db, PolicyAdminConstants.USER);
        	return;
        }
			
		new SetUserPasswordRequestHandler().updateUserPasswordInDB(request, username, oldPassword, newPassword, MRXMLUtils.elementToDocument(userEL), xmpp, log, db);
	
	}
	
}
