package com.barco.device.vng.DeviceAdmin.deviceengine;

import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Node;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.vng.DeviceAdmin.DeviceAdminEngineObserverI;
import com.barco.device.vng.DeviceAdmin.DeviceAdminExceptionCodes;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.utils.FileUtils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;

/**
 * The engine synchronizes the device config and status document with the 
 * running system, by running shell scripts.
 * <p>
 * The engine delegates command (reboot, ...) to OS shell scripts (through
 * {@link CommandLineUtils}).
 * <p>
 * The engine runs a cli server, accepts and handles commands (CLIConfig,
 * CLIStatus, SystemNotification) and notifies the engine-observers.
 * <p>
 * Engine.setDeviceConfig() applies a config to the running system by running a
 * bunch of shell scripts.
 */
public class DeviceAdminEngineImpl extends BaseDeviceAdminEngineImpl {

	private static final String GET_STREAMING_STATUS_CLI_CMD = "get_streaming_status";
	public DeviceAdminEngineImpl(Logger log, DeviceDBI deviceDB){
		super(log, deviceDB);	
	}

	/**
	 * Applies the deviceConfigDoc to the running system.
	 * 
	 * Each handler in /com/ipvs/xsd/xml/confighandler.xml gets to handle the
	 * deviceConfigDoc (this.applyHandler), and contributes to a result.xml,
	 * which is checked for correctness and then thrown away.
	 */
	@Override
    public void setDeviceConfig(Document deviceConfigDoc) throws Exception {

		Element dochandlerElement = getHandlerDocument(DeviceAdminEngineConst.CFGHANDLER);
		Element commandHandlerEl = null;

		Iterator<?> iter = dochandlerElement.elements().iterator();
		Document resultDoc=XMLUtils.stringToDocument("<Result/>");

		while (iter.hasNext()) {
			commandHandlerEl = (Element) iter.next();
			applyHandler(deviceConfigDoc, commandHandlerEl, log,resultDoc);
		}	

		if(XMLUtils.getElementXpath(resultDoc, "//Result/*[.='"+DeviceAdminEngineConst.STATE_EXCEPTION+"']")!=null) {
			throw new ServiceException(DeviceAdminExceptionCodes.CAN_NOT_APPLY_CONFIG,"Can not Apply given config");        	
		}

	}

	/**
	 * Apply the given config on local device xml by running a shell script in
	 * /V2O/bin, specified by /com/ipvs/xsd/xml/confighandler.xml, and produces
	 * changes in resultdoc.
	 */
	public boolean applyHandler(Document deviceConfigDoc, Element commandHandlerEl, Logger log,Document resultdoc) {
		Element el = null;
		String trigger = commandHandlerEl.elementText("triggername");
		String expectResponse = commandHandlerEl.elementText("expectResponse");
		boolean bsendClientData = false;


		List<?> triggerElementList = null;

		String operation =XMLUtils.getValue(commandHandlerEl,"action","");
		String dirtyFlag ="Unknown";
		String listIndex=null;

		ArrayList<String> cmdArray = new ArrayList<String>();

		if ((expectResponse != null) && (expectResponse.compareToIgnoreCase("yes") == 0)) {
			bsendClientData = true;
		}

		try {
			triggerElementList = DocumentHelper.createXPath(trigger).selectNodes(deviceConfigDoc);			
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (triggerElementList != null && !triggerElementList.isEmpty()) {

			for (int i = 0; i < triggerElementList.size(); i++) {
				// Do nothing if the flag does not match the 
				// action in the command except for startup : Shanker 11/12008

				Element triggerElement = (Element) triggerElementList.get(i);
				try {
					dirtyFlag = XMLUtils.getAttributeValue(triggerElement,"Dirty");
				}
				catch (Exception e) {
					dirtyFlag=operation;
				}			


				if (dirtyFlag == null){
					dirtyFlag = DeviceAdminConst.INIT_FLAG;
				}
				if (dirtyFlag.compareToIgnoreCase( DeviceAdminConst.INIT_FLAG)==0) {
					if (operation.compareToIgnoreCase(DeviceAdminEngineConst.ADD_OPERATION)==0) {
						dirtyFlag = operation;
					}
					else if (operation.compareToIgnoreCase(DeviceAdminEngineConst.MODIFY_OPERATION)==0) {
						dirtyFlag = operation;
					}
				}
				if (dirtyFlag.compareToIgnoreCase(operation)!=0) {
					continue;
				}

				//StringBuffer sb = new StringBuffer();
				//sb.append("echo ");
				cmdArray.clear();
				cmdArray.add(XMLUtils.getValue(commandHandlerEl,"cmd",""));

				if (bsendClientData) {
					cmdArray.add("-clientData");
					cmdArray.add("001");
				}

				if (operation.compareToIgnoreCase("DELETE")==0) {
					try {
						listIndex =XMLUtils.getAttributeValue( triggerElement,"ListIndex");
					}
					catch (Exception e) {
						listIndex="0";
					}
					cmdArray.add(listIndex);
				}
				if (operation.compareToIgnoreCase("UPDATELIST")==0) {
					try {
						listIndex = XMLUtils.getAttributeValue(triggerElement,"ListIndex");
					}
					catch (Exception e) {
						listIndex="0";
					}
					cmdArray.add(listIndex);
				}
				// First prepend the commands
				String parentXpath="";
				String parentValue="";
				String parentArg="";
				try {
					parentXpath=XMLUtils.getAttributeValue(commandHandlerEl.element("parent"),"relativeXpath");
					parentArg=XMLUtils.getAttributeValue(commandHandlerEl.element("parent"),"cliargument");
					parentValue=XMLUtils.getValueXpath(triggerElement,parentXpath, "unknown");
					cmdArray.add("-" + parentArg);
					cmdArray.add(parentValue);
				}
				catch (Exception e) {
					// Do nothing some commands do not have parents
				}

				Iterator<?> iter = commandHandlerEl.element("paramtable").elements().iterator();

				while (iter.hasNext()) {
					el = (Element) iter.next();

					String token = el.elementText("paramval");
					String val = null;

					try {
						val = XMLUtils.getValueXpath(triggerElement, token, null);
					} catch (Exception e) {
						e.printStackTrace();
					}

					if (val != null) {
						cmdArray.add("-" + el.elementText("paramname"));

						// Fix Booleans -- map true/false to Yes/No
						String type = XMLUtils.getAttributeValue(el,"type");
						if (type == null) {
							type = "String";
						}
						if (type.compareTo("Boolean") == 0) {
							if (val.compareToIgnoreCase("true") == 0) {
                                val = "Yes";
                            } else if (val.compareToIgnoreCase("false") == 0) {
								val = "No";
							}
						}
						else if (type.compareTo("URLString") == 0) {
							try {
								val = URLEncoder.encode(val, "UTF-8");
							} catch (UnsupportedEncodingException e) {
							}
						}
						cmdArray.add(val);
					} else {
						log.info("applyHandler: Could not find value for " + XMLUtils.getValue(commandHandlerEl,"cmd","") + "  -> " + token);
					}
				}

				log.info("applyHandler calling ExecuteCLICommand: "+cmdArray.toString());
				int state = DeviceAdminEngineConst.STATE_DONE;
				try {
					String result = executeDeviceCommand( cmdArray.toArray(new String[cmdArray.size()]));
					log.info("applyHandler called ExecuteCLICommand: result "+ result);
				} catch (Exception e) {					
					state = DeviceAdminEngineConst.STATE_EXCEPTION;
					e.printStackTrace();
				}
				Element resultEl = DocumentHelper.createElement(triggerElement.getName());
				try {
					resultEl.setText( ""+state);
					resultdoc.getRootElement().add(resultEl);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		return false;
	}

	/**
	 * Executes the script in /V2O/bin/
	 */
	private String executeDeviceCommand(String[] cmdArray) throws Exception {
		cmdArray[0] = "/V2O/bin/" + cmdArray[0];
		System.out.println("Command Array:="+cmdArray[0]);
		return CommandLineUtils.executeCLICommand(cmdArray);
	}



	/**
	 * Handles the cli command (CLIConfig, CLIStatus, SystemNotification),
	 * and notifies the engine observers.
	 */
	@Override
    protected void handleNotification(String notification) {
		if ((notification == null) || notification.isEmpty()) {
			return;
		}

		try {
			Document notificationDoc = XMLUtils.stringToDocument(notification);
			String notificationType = notificationDoc.getRootElement().getName();           
			if (DeviceAdminEngineConst.CLI_CONFIG.equals(notificationType)) {
				handleconfigUpdate(notificationDoc);
			}
			else if (DeviceAdminEngineConst.CLI_STATUS.equals(notificationType)) {
				handlestatusUpdate(notificationDoc);
			} else if(DeviceAdminEngineConst.SYSTEM_NOTIFICATION.equals(notificationType)) {
				String eventSource = DocumentHelper.createXPath("//EventSource").selectSingleNode(notificationDoc).getText();			
				callObserver(eventSource+"Event", notificationDoc.getRootElement());	
				//TODO: This code required to change when implementing dms agents					
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	@SuppressWarnings("unchecked")
	private void handlestatusUpdate(Document notificationDoc) {

		Element el = null;
		Document dochandler = null;
		Document docStatus = null;
		String cmd=null;
		String deviceXML=null;
		String updateXPath=null;
		String tableName =null;
		String elementName= null;
		String newErrorState = null;
		
		
	
		try {
			dochandler = XMLUtils.loadXMLResource(DeviceAdminEngineConst.STATUSHANDLER);
			cmd = XMLUtils.getValueXpath(notificationDoc, "//CLICommand", null);
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}
		
		//testing code to send port active inactive notifications
		if(cmd.equals("get_tx_connection_status")) {
			try {
				Element deviceStatusEl = DocumentHelper.createElement("DeviceStatus");
				String portID = XMLUtils.getValueXpath(notificationDoc, "//ChannelNumber", "1");
				String horizonLines = DocumentHelper.createXPath("//Parameter[@name='Horiz. Lines']").selectSingleNode(notificationDoc).getText();
				String newState = "Active";
				if(horizonLines.equals("0")) {
					 newState = "Inactive";
				}
				
				deviceStatusEl.addAttribute("xpath", "//AVCodecElementsStatus/*/.[PortID="+portID+"]/@State");
				deviceStatusEl.addAttribute("operation", "UpdateAttribute");
				Element attEl = DocumentHelper.createElement("Attribute");
				attEl.addAttribute("name", "State");
				attEl.addAttribute("value", newState);
				deviceStatusEl.add(attEl);
				callObserver(DeviceAdminConst.DEVICE_STATUS_UPDATE_EVENT, deviceStatusEl);
				return;
			} catch (Exception e) {	
				return;
			}	
		}


		try {
			tableName =XMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "']/tablename", null);
			updateXPath =XMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "']/triggername", "");
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			String locator = null;
			String locatorValue = null;
			locator = XMLUtils.getElementXpath(dochandler, "//handler[cmd='" + cmd + "']/parent").attributeValue("relativeXpath", null);
			locatorValue = XMLUtils.getValueXpath(notificationDoc, 
					"//" + 
							XMLUtils.getElementXpath(dochandler, "//handler[cmd='" + cmd + "']/parent").attributeValue("name", null), 
					"1");
			if (locator != null) {
				updateXPath += "[" + locator + "=" +  locatorValue + "]";
			}
		} catch (Exception e1) {
			e1.printStackTrace();
		}

		try {
			elementName =XMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "']/elementname", "");
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (tableName != null) {
			try {
				el = DocumentHelper.createElement(tableName);
				Element child=null;
				Element childdata=null;
				List<?> l=null;
				try {
					l =   DocumentHelper.createXPath( "//Parameters").selectNodes(notificationDoc);
				} catch (Exception e) {
					e.printStackTrace();
				}

				if ((l != null) && !l.isEmpty()) {
					for (int i = 0; i < l.size(); i++) {
						child = DocumentHelper.createElement(elementName);
						childdata = (Element) l.get(i);
						child.add((Element)childdata.clone());
						el.add(child);
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		else {
			Element childdata=null;
			try {
				el =  DocumentHelper.createElement("Childdata");
				childdata = XMLUtils.getElementXpath(notificationDoc, "//Parameters");
				el.add((Element)childdata.clone());
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		try {
			newErrorState = matchErrorState(DeviceAdminEngineConst.FAILED, cmd, dochandler, notificationDoc);
			if(newErrorState == null) {
				newErrorState = matchErrorState(DeviceAdminEngineConst.ERRORS, cmd, dochandler, notificationDoc);
				if(newErrorState == null) {
					newErrorState = DeviceAdminEngineConst.OK;
				}
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		try {
			deviceXML = FileUtils.readFile(DeviceAdminEngineConst.DEVICESTATUSFILE);
			docStatus = XMLUtils.stringToDocument(deviceXML);
			Element updateEl  = XMLUtils.getElementXpath(docStatus, "//DeviceStatus/" + updateXPath);
			updateEl.clearContent();
			updateEl.content().addAll(((Element)el.clone()).content());
			if(newErrorState != null) {
				String deviceState = XMLUtils.getElementXpath(docStatus, "//DeviceStatus/" + updateXPath).attributeValue("State","");
				if(!deviceState.equals(newErrorState)) {  
					Element element = XMLUtils.getElementXpath(docStatus, "//DeviceStatus/" + updateXPath);
					element.addAttribute("State", newErrorState);
					//TODO Send Status updates to Agent / Device Interface to send message out
					//app.handleAgentErrorStatusUpdate(element, newErrorState);
					while((element != null) && (!element.getName().equals("DeviceStatus"))) {
						Element parentEl = element.getParent();
						String newParentState = findNewParentState(parentEl);
						String oldParentState = XMLUtils.getAttributeValue(parentEl, "State");
						if(newParentState.equals(oldParentState)) {
							break;
						}
						parentEl.addAttribute( "State", newParentState);
						//TODO Send Status updates to Agent / Device Interface to send message out
						//app.handleAgentErrorStatusUpdate(parentEl, newParentState);
					
						Element deviceStatusEl = DocumentHelper.createElement("DeviceStatus");
						deviceStatusEl.addAttribute("xpath", "//"+ parentEl.getName() +"/@State");
						deviceStatusEl.addAttribute("operation", "UpdateAttribute");
						Element attEl = DocumentHelper.createElement("Attribute");
						attEl.addAttribute("name", "State");
						attEl.addAttribute("value", newParentState);
						deviceStatusEl.add(attEl);						
						element = parentEl;
						callObserver(DeviceAdminConst.DEVICE_STATUS_UPDATE_EVENT, deviceStatusEl);	
					}
				}
			}
			FileUtils.writeFile(DeviceAdminEngineConst.DEVICESTATUSFILE,XMLUtils.documentToString(docStatus));

			//send status update event
			Element deviceStatusEl = DocumentHelper.createElement("DeviceStatus");
			deviceStatusEl.addAttribute("xpath", "//DeviceStatus/" + updateXPath);
			deviceStatusEl.addAttribute("operation", "Update");
			deviceStatusEl.add((Element)updateEl.clone());
			callObserver(DeviceAdminConst.DEVICE_STATUS_UPDATE_EVENT, deviceStatusEl);						

		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void handleconfigUpdate(Document notificationDoc) {
		Element el = null;

		Document dochandler = null;
		Document docbasecfg = null;
		Document docrefcfg = null;
		String defaultConfigFile;
		Element deviceConfig = null;
		String deviceName=null;

		try {
			el = notificationDoc.getRootElement();
			System.out.println("Command Status:="+el.asXML());

			// Temporary hack by Shanker to discard _auth_ commands
			// Oct 10/13
			String cmd = XMLUtils.getValueXpath(el, "//CLICommand", null);

			// Handle failure cases : Shanker
			// Currently just silently discard
			String status = XMLUtils.getValueXpath(el, "/CLIConfig/Command/Status", "Failure");

			//System.out.println ("UpdateDeviceConfig :" + status);
			if (status.compareToIgnoreCase("Success") != 0) {
				log.info("updateDeviceConfig Ignoring ERROR " + cmd);
				return;
			}

			if (cmd.compareToIgnoreCase("set_tx_connection_parm") == 0) {
				//System.out.println("updateDeviceConfig Ignoring command " + cmd);
				return;
			}

			dochandler = XMLUtils.loadXMLResource(DeviceAdminEngineConst.CFGHANDLER);
			defaultConfigFile = DeviceAdminEngineConst.BASECFGPREFIX + deviceDB.getDeviceType() + DeviceAdminEngineConst.BASECFGSUFFIX;
			docrefcfg = XMLUtils.loadXMLResource(defaultConfigFile);			
			docbasecfg = deviceDB.getDeviceConfig();
			deviceConfig = docbasecfg.getRootElement();

			if (deviceConfig != null) {
				deviceName = deviceDB.getDeviceType();
			}
			String doDBUpdate=XMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/dbupdate", "true");
			String doReset=XMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/xmppreset", "false");
			String updateXPath = XMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/triggername", null);
			String action = XMLUtils.getValueXpath(dochandler,
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/action", null);
			String updateParentXPath=null;
			String operation = "Update";
			if (action.compareToIgnoreCase("ADD")==0) {
				updateParentXPath = XMLUtils.getValueXpath(dochandler, 
						"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/tablename", null);
				operation = "Add";
			} if(action.compareToIgnoreCase("Delete")==0){
				operation = "Delete";
			}


			if (updateXPath == null) {
				log.info("updateDeviceConfig Ignoring Command " + cmd);
				return;
			}
			try {
				String locatorName=null;
				String locatorRef=null;
				String tableLocatorRef=null;
				String locatorValue=null;
				String indexValue=null;
				Element eh=null;
				eh = XMLUtils.getElementXpath(dochandler, 
						"//handler[cmd='" + cmd + "' and device='" + deviceName +"']");					
				Element parentEl = eh.element("parent");
				if(parentEl != null) {
					locatorRef = XMLUtils.getAttributeValueWithDefault(parentEl, "relativeXpath", null);				
					if (locatorRef != null) {
						locatorName = XMLUtils.getAttributeValueWithDefault(parentEl, "name", null);
						locatorValue = XMLUtils.getValueXpath(el, "//"+ locatorName, null);
						updateXPath += "[" + locatorRef +"=" + locatorValue + "]";
						if (updateParentXPath != null) {
							tableLocatorRef = XMLUtils.getAttributeValueWithDefault(parentEl, "tablerelativeXpath", null);
							updateParentXPath += "[" + tableLocatorRef +"='" + locatorValue + "']";
						}
					}
				}
				indexValue = XMLUtils.getValueXpath(el, "//Command/Index", null);
				if (indexValue != null) {
					updateXPath += "[@ListIndex=" + indexValue + "]";
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

			//added by pravin to set flag to dirty=clean if empty
			updateDirtyChildrenToClean(docbasecfg, log);

			if (el != null) {
				updateConfig(el, dochandler, docbasecfg, docrefcfg);				
				deviceDB.setDeviceConfig(docbasecfg);				

				if (doDBUpdate.compareToIgnoreCase("true")==0) {
					Element deviceConfigEl = DocumentHelper.createElement("DeviceConfig");
					String xpath = updateXPath;
					if(operation.equals("Add")) {
						xpath = updateParentXPath;
					}					
					deviceConfigEl.addAttribute("operation", operation);
					deviceConfigEl.addAttribute("xpath", xpath);
					if(!operation.equals("Delete")) {
                        deviceConfigEl.add((Element)DocumentHelper.createXPath(updateXPath).selectSingleNode(docbasecfg).clone());
                    }
					if(operation.equals("Update") && xpath.startsWith("//XP200PortConfig/Info") && XMLUtils.getValueXpath(deviceConfigEl, ".//Info/PortIOType", "").equals("StreamSrc")){
					    deviceConfigEl.addAttribute("thumbnail", "true");
					}
					callObserver(DeviceAdminConst.DEVICE_CONFIG_UPDATE_EVENT, deviceConfigEl);						
				}		
				
				if (doReset.compareToIgnoreCase("true")==0) {
					Element loginEventEl = DocumentHelper.createElement("LoginEvent");
					callObserver(DeviceAdminConst.LOGIN_EVENT, loginEventEl);	
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void callObserver(String eventType,Element eventElement) {
		for(DeviceAdminEngineObserverI engineObserver:deviceServerObserverList) {
			engineObserver.handleEngineNotification(eventType, DocumentHelper.createDocument((Element)eventElement.clone()));
		}
	}

	public void updateDirtyChildrenToClean(Document updatedoc, Logger log){
		try{
			String xPath = "";
			List<?> dirtyList = null;
			xPath = "//*[@" + DeviceAdminConst.DIRTY_TAG + "]";
			dirtyList = DocumentHelper.createXPath(xPath).selectNodes(updatedoc);
			for (Object name : dirtyList) {
				Element dirtyEL = (Element) name;
				String attrb = XMLUtils.getAttributeValueWithDefault(dirtyEL, DeviceAdminConst.DIRTY_TAG,null);
				if (attrb != null &&   attrb.trim().equalsIgnoreCase("")  ) {
					XMLUtils.addAttribute(dirtyEL, DeviceAdminConst.DIRTY_TAG,DeviceAdminConst.CLEAN_FLAG);
				} 
			}
		}catch(Exception ex){
			ex.printStackTrace();
		}
	}

	public synchronized Element updateConfig(Element el, Document dochandler, Document docbasecfg, Document docrefcfg) {
		String cmd = null;
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
			deviceName = deviceConfig.getName();
		}

		cmd = el.element("Command").element("CLICommand").getStringValue();

		if (cmd == null) {
			return null;
		}

		xmlsnippet = DocumentHelper.createElement(cmd);

		Iterator<?> params = el.element("Parameters").elements().iterator();
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
			apply = XMLUtils.getElementXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']");

			action = apply.element("action").getStringValue();

			log.info("updateConfig :" + cmd + "->" + action);


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
				indexattr = "[@ListIndex=\"" + el.element("Command").element("Index").getStringValue() + "\"]";
				log.info("updateConfig :" + action + indexattr);
			}
			// Get Locator Lists
			String locatorName=null;
			String locatorRef=null;
			if (action.compareToIgnoreCase("add") == 0) {
				locatorXpath=table;
				try {
					locatorName=apply.element("parent").attributeValue("name");
					locatorRef=apply.element("parent").attributeValue("tablerelativeXpath");
					String locatorValue= el.element("Command").element(locatorName).getStringValue();
					locatorXpath += "[" + locatorRef +"=\"" 
							+ locatorValue + "\"]";
				}
				catch (Exception e) {
				}
			}
			else {
				locatorXpath=trigger;
				try {
					locatorName=apply.element("parent").attributeValue("name");
					locatorRef=apply.element("parent").attributeValue("relativeXpath");
					String locatorValue= el.element("Command").element(locatorName).getStringValue();
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
					log.info("updateConfig :Warning: Name not found " + cmd + ":" + name);
				}
			}

			applyargs = xmlsnippet.elements().iterator();

			while (applyargs.hasNext()) {
				child = (Element) applyargs.next();

				if (child.attributeValue("xpath") == null) {
					log.info("updateConfig :Warning Ignoring " + cmd + ":" + child.getName());
				}
			}
		}
		catch (Exception e) {
		}

		if (matched) {
			Document doc = null;
			Element c1 = null;

			if (action.compareToIgnoreCase("add") == 0) {
				// Get the element from the object Xml
				// MRXMLUtils.loadXMLResource(xmlFilePath, log)
				// Find the table entry within the Device XML
				String index = el.element("Command").element("Index").getStringValue();
				try {
					doc=XMLUtils.loadXMLResource(defaultFile);
					c1 = (Element) DocumentHelper.createXPath(locatorXpath).selectSingleNode(deviceConfig);	
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

			log.info("updateConfig :Updated Command " + cmd);
		} else {
			if (found == false) {
				log.info("updateConfig :Error Could not handle :" + cmd);
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
			l = XMLUtils.selectNodesXpath(deviceConfig, trigger);
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

	private  String findNewParentState(Element parentEl) throws Exception {
		List<?> failedList = XMLUtils.selectNodesXpath(parentEl, "./*[@State='" + DeviceAdminEngineConst.FAILED + "']");
		if((failedList != null) && (!failedList.isEmpty())) {
			return DeviceAdminEngineConst.FAILED;
		}

		List<?> errorList = XMLUtils.selectNodesXpath(parentEl, "./*[@State='" + DeviceAdminEngineConst.ERRORS + "']");
		if((errorList != null) && (!errorList.isEmpty())) {
			return DeviceAdminEngineConst.ERRORS;
		}
		return DeviceAdminEngineConst.OK;
	}


	public String matchErrorState(String type, String cmd, Document dochandler, Document notificationDoc) throws Exception{
		List<?> errorList = DocumentHelper.createXPath("//handler[cmd='" + cmd + "']/error[@type='" + type + "']").selectNodes(dochandler);
		if((errorList != null) && !errorList.isEmpty()) {
			for(Object error:errorList) {
				Element errorEl = (Element)error;
				String errorName = XMLUtils.getAttributeValueWithDefault(errorEl, "name", "");
				String errorValue = XMLUtils.getAttributeValueWithDefault(errorEl, "value", "");
				String errorType = XMLUtils.getAttributeValueWithDefault(errorEl, "type", "");
				String xpath = "//Parameter[@name='" + errorName + "' and .='" + errorValue + "']";
				List<?> l =  DocumentHelper.createXPath(xpath).selectNodes(notificationDoc);
				if(l != null && !l.isEmpty()) {
					return errorType;
				}
			}
		}
		return null;
	}

	/**
	 * Update the element with the latest status by querying the running system.
	 */
	@Override
	public void getDeviceStatus(Element tempDeviceStatusElement) throws Exception {
		Element    statusHandlerDoc = getHandlerDocument(DeviceAdminEngineConst.STATUSHANDLER);
		List<?>    statusHandlerChildList = statusHandlerDoc.elements();
		if ((statusHandlerChildList != null) && (statusHandlerChildList.size() != 0)) {
			tempDeviceStatusElement = getDeviceStatus(log, tempDeviceStatusElement, statusHandlerChildList);
		}
	}

	/**
	 * For each handler in /com/ipvs/xsd/xml/statushandler.xml, the cli cmd is
	 * run, the xml-output is parsed, and the appropriate element
	 * (triggername)is replaced in the tempDeviceStatusElement document.
	 * 
	 * @return deviceStatus (internally modified)
	 */
	private Element getDeviceStatus(Logger log, Element deviceStatus, List<?> statusHandlerChildList) {
		//System.out.println("In get device status");
		String cmd = null;
		String triggerName = null;
		Element paramListEL = null;
		List<?> paramList = null;
		Document cmdResultDoc = null;
		Element statusEL = null;
		String elementname ="";		
		for (Object name : statusHandlerChildList) {
			Element childEL = (Element) name;
			cmd = childEL.elementText("cmd");
			triggerName = childEL.elementText("triggername");
			paramListEL = childEL.element("paramtable");
			elementname = childEL.elementText("elementname");
			if (paramListEL != null) {
				paramList = childEL.elements();
				cmdResultDoc = executeCommandWithParameters(log, cmd, paramList);
			} else {
				cmdResultDoc = executeCommandWithOutParameters(log, cmd);
			}

			// TBD: Need to check CLI command output XML
			if (cmdResultDoc != null) {
				try {
					if(elementname !=null && !elementname.isEmpty()) {
						triggerName += "/"+elementname;
					}
					statusEL = (Element) DocumentHelper.createXPath(triggerName).selectSingleNode(deviceStatus);
					// System.out.println("Status xml before update\n" + MRXMLUtils.elementToString(statusEL));
					Element parametersEL = null;
					parametersEL = (Element) (XMLUtils.getElementXpath(cmdResultDoc, "//Status").detach());

					//System.out.println("DATA " + MRXMLUtils.elementToString(parametersEL));
					if (parametersEL != null) {
						Element data = (Element) parametersEL.elements().get(0); 

						if (((data != null) && (statusEL != null))) {

							String parametersElementName = data.getName();
							Element statusParamEL = statusEL.element(parametersElementName);
							if(statusParamEL != null) {
								statusParamEL.detach();
							}	

							statusEL.add((Element)data.detach());
						}
					}
				} catch (Exception e) {
					log.info("Exception occur when get the new status from executed commands in getDeviceStatus() method  : \n" + e.getMessage());
					e.printStackTrace();
				}
			}
		}
		try {			
			String[] cmdArray = { GET_STREAMING_STATUS_CLI_CMD };
			Element ssInstanceList = DocumentHelper.createElement("InstanceList");
			 
			Document ssDetailsDoc = executeDeviceStatusCommands(cmdArray);			
			Element el = null;
			try {
        		el = XMLUtils.getElementXpathFromElement(deviceStatus,"//StreamingServiceStatus/InstanceList");
        		el.detach();
        	}
        	catch (Exception e) {
        	}
			//add the ssDetailsDoc to deviceStutus here, in getDeviceStatusRequestHandler, call the foolowing
			//piece of code to get the desttable 
			List<?> parametersList = XMLUtils.getListOfNodesXpath(ssDetailsDoc,
					"//Parameters");
			Element parametersEL = null;
			String ssInstanceId = null;
			Iterator<?> parametersListIT = parametersList.iterator();

			while (parametersListIT.hasNext()) {
				Element ssElement = null;
				parametersEL = (Element) parametersListIT.next();
				ssInstanceId = XMLUtils.getValueXpath(parametersEL,
						"./Parameter[@name='Id']", "");
				String ssInstanceType = XMLUtils.getValueXpath(parametersEL,
						"./Parameter[@name='Type']", "");
				if(ssInstanceType.equals(MediaRoomServiceConst.V2D_ENCODER_PORT)){
					ssElement = DocumentHelper.createElement("V2DEncoderInstance");
					ssElement.addAttribute("NID", "StreamingServiceStatus.InstanceList."+ssInstanceId);
				}
				if(ssInstanceType.equals(MediaRoomServiceConst.V2D_DECODER_PORT)){ //<V2DStreamTxInstance
					ssElement = DocumentHelper.createElement("V2DStreamTxInstance");
					ssElement.addAttribute("NID", "StreamingServiceStatus.InstanceList."+ssInstanceId);
				}
				if(ssInstanceType.equals(MediaRoomServiceConst.V2D_NETWORK_PORT) ||ssInstanceType.equals(MediaRoomServiceConst.V2D_HARDWARE_DECODER_PORT) ){ //<V2DStreamTxInstance
					ssElement = DocumentHelper.createElement("V2DStreamRxInstance");
					ssElement.addAttribute("NID", "StreamingServiceStatus.InstanceList."+ssInstanceId);
				}
				
				else {
					ssElement = DocumentHelper.createElement("SSInternalInstance");
					ssElement.addAttribute("NID", ssInstanceId);
				}
				parametersEL.detach();
				ssElement.add(parametersEL);
				ssInstanceList.add(ssElement);
			}		
			Element streamingServiceStatus = XMLUtils.getElementXpathFromElement(deviceStatus, ".//StreamingServiceStatus");
        	if (ssInstanceList != null && streamingServiceStatus != null) {        	
        		streamingServiceStatus.add(ssInstanceList);
        	}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return deviceStatus;
	}

	private Document executeCommandWithParameters(Logger log, String cmd, List<?> paramList) {
		// StringBuffer command = new StringBuffer();
		ArrayList<String> cmdArray = new ArrayList<String>();

		cmdArray.add(cmd);

		String param = null;
		String paramValue = null;

		for (Object name : paramList) {
			Element paramEL = (Element) name;

			try {
				param = paramEL.elementText("paramname");
				paramValue = paramEL.elementText("paramval");

				if ((param != null) && (param.length() != 0)) {
					cmdArray.add("-" + param);
					cmdArray.add(paramValue);
				}
			} catch (Exception exp) {
				log.info("Exception occur when executing the CLI command in executeCommandWithParameters() method  : \n" + exp.getMessage());
				exp.printStackTrace();
			}
		}

		//System.out.println("command  " + command.toString());
		return executeDeviceStatusCommands(cmdArray.toArray(new String[cmdArray.size()]));
	}

	//Execute the CLI command with out parameters list
	private  Document executeCommandWithOutParameters(Logger log, String cmd) {
		String[] cmdArray = { cmd };

		return executeDeviceStatusCommands(cmdArray);
	}


	private Document executeDeviceStatusCommands(String[] cmdArray) {
		String cmdresult = null;

		try {
			cmdresult = CommandLineUtils.executeCLICommand(cmdArray);

			int index = cmdresult.indexOf("<");

			if (index != -1) {
				cmdresult = cmdresult.substring(index).trim();
			}
		} catch (Exception e) {
			log.info("Exception occur when executing the CLI command in executeCLICommand() method  : \n" + e.getMessage());
		}

		if ((cmdresult != null) && (cmdresult.length() != 0)) {
			try {
				Document cmdResultdoc =XMLUtils.stringToDocument(cmdresult);

				String status = XMLUtils.getValueXpath(cmdResultdoc, "//Command/Status", "");

				if (status.equals("Success")) {
					return cmdResultdoc;
				} else {
					return null;
				}
			} catch (Exception e) {
				log.info("Exception occur when executing the ELI command in executeCLICommand() method  : \n" + e.getMessage());
				e.printStackTrace();
			}
		}

		return null;
	}

	//load the xml document 
	private Element getHandlerDocument(String handlerXML) throws Exception {
		Document handlerDoc = null;

		try {
			handlerDoc = XMLUtils.loadXMLResource(handlerXML);

			if (handlerDoc != null) {
				return handlerDoc.getRootElement();
			}
		} catch (Exception exp) {
			throw new ServiceException(DeviceAdminExceptionCodes.HANDLER_NOT_FOUND,"Device Config File Not Found");
		}

		return null;
	}

	@Override
	public void resetServer() {
		setResetServer(true);
	}


	@Override
	public void reboot()  throws Exception{		
		String[] command = {"reboot_system"};
		executeDeviceCommand(command);
	}


	@Override
	public void upgrade(String url)   throws Exception {
		String[] command = {"upgrade_device", "-url", url};		
		executeDeviceCommand(command);
	}


	@Override
	public void revertToDefault(boolean revertNetworkConfig, boolean deleteMedia)  throws Exception{
		String revertNetWorkCofigStr = DeviceAdminEngineConst.NO;
		if(revertNetworkConfig) {
			revertNetWorkCofigStr = DeviceAdminEngineConst.YES;
		}

		String deleteMediaStr = DeviceAdminEngineConst.NO;
		if(deleteMedia) {
			deleteMediaStr = DeviceAdminEngineConst.YES;
		}

		String[]  command = {"revert_config", "-reset_network" , revertNetWorkCofigStr , "-delete_media" ,deleteMediaStr};
		executeDeviceCommand(command);
	}


	@Override
	public void saveConfiguration()   throws Exception {
		String[] command = {"save_config"};
		executeDeviceCommand(command);
	}


	@Override
	public void shutdown() throws Exception {
		String[] command = {"shutdown_system"};
		executeDeviceCommand(command);
		
	}

	@Override
	public void haTakeover() throws Exception {
		String[] command = {"ha_takeover"};
		log.info("Executing command: " + command.toString());
		executeDeviceCommand(command);		
	}

}
