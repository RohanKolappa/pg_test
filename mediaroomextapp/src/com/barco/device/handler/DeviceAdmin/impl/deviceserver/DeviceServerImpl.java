package com.barco.device.handler.DeviceAdmin.impl.deviceserver;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.handler.DeviceAdmin.impl.DeviceAdminConst;
import com.barco.device.handler.DeviceAdmin.impl.DeviceAdminExceptionCodes;
import com.barco.utils.CommandLineUtils;
import com.barco.utils.FileUtils;
import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRRequest;
import com.barco.utils.mr.MRXMLUtils;

public class DeviceServerImpl extends BaseDeviceServerImpl {

	
	public DeviceServerImpl(MRLog log, String configFile){
		super(log, configFile);	
	}
	

	public void setDeviceConfig(Document deviceConfigDoc) throws Exception {

		Element dochandlerElement = getHandlerDocument(DeviceServerConst.CFGHANDLER);
		Element commandHandlerEl = null;

		Iterator<?> iter = dochandlerElement.elements().iterator();
		Document resultDoc=MRXMLUtils.stringToDocument("<Result/>");

		while (iter.hasNext()) {
			commandHandlerEl = (Element) iter.next();
			applyHandler(deviceConfigDoc, commandHandlerEl, log,resultDoc);
		}	

		if(MRXMLUtils.getElementXpath(resultDoc, "//Result/*[.='"+MRRequest.STATE_EXCEPTION+"']")!=null) {
			throw new MRException(DeviceAdminExceptionCodes.CAN_NOT_APPLY_CONFIG,"Can not Apply given config");        	
		}

	}

	/**
	 * Apply the given config on local device xml using cli command
	 * @param deviceConfigDoc
	 * @param commandHandlerEl
	 * @param log
	 * @param resultdoc
	 * @return
	 */
	public boolean applyHandler(Document deviceConfigDoc, Element commandHandlerEl, MRLog log,Document resultdoc) {
		Element el = null;
		String trigger = commandHandlerEl.elementText("triggername");
		String expectResponse = commandHandlerEl.elementText("expectResponse");
		boolean bsendClientData = false;


		List<?> triggerElementList = null;

		String operation =MRXMLUtils.getValue(commandHandlerEl,"action","");
		String dirtyFlag ="Unknown";
		String listIndex=null;

		ArrayList<String> cmdArray = new ArrayList<String>();

		if ((expectResponse != null) && (expectResponse.compareToIgnoreCase("yes") == 0)) {
			bsendClientData = true;
		}

		try {
			triggerElementList = MRXMLUtils.getListXpath(deviceConfigDoc, trigger);
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (triggerElementList != null && !triggerElementList.isEmpty()) {

			for (int i = 0; i < triggerElementList.size(); i++) {
				// Do nothing if the flag does not match the 
				// action in the command except for startup : Shanker 11/12008

				Element triggerElement = (Element) triggerElementList.get(i);
				try {
					dirtyFlag = MRXMLUtils.getAttributeValue(triggerElement,"Dirty");
				}
				catch (Exception e) {
					dirtyFlag=operation;
				}			


				if (dirtyFlag == null){
					dirtyFlag = DeviceAdminConst.INIT_FLAG;
				}
				if (dirtyFlag.compareToIgnoreCase( DeviceAdminConst.INIT_FLAG)==0) {
					if (operation.compareToIgnoreCase(DeviceServerConst.ADD_OPERATION)==0) {
						dirtyFlag = operation;
					}
					else if (operation.compareToIgnoreCase(DeviceServerConst.MODIFY_OPERATION)==0) {
						dirtyFlag = operation;
					}
				}
				if (dirtyFlag.compareToIgnoreCase(operation)!=0) {
					continue;
				}

				//StringBuffer sb = new StringBuffer();
				//sb.append("echo ");
				cmdArray.clear();
				cmdArray.add(MRXMLUtils.getValue(commandHandlerEl,"cmd",""));

				if (bsendClientData) {
					cmdArray.add("-clientData");
					cmdArray.add("001");
				}

				if (operation.compareToIgnoreCase("DELETE")==0) {
					try {
						listIndex =MRXMLUtils.getAttributeValue( triggerElement,"ListIndex");
					}
					catch (Exception e) {
						listIndex="0";
					}
					cmdArray.add(listIndex);
				}
				if (operation.compareToIgnoreCase("UPDATELIST")==0) {
					try {
						listIndex = MRXMLUtils.getAttributeValue(triggerElement,"ListIndex");
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
					parentXpath=MRXMLUtils.getAttributeValue(MRXMLUtils.getChild(commandHandlerEl,"parent"),"relativeXpath");
					parentArg=MRXMLUtils.getAttributeValue(MRXMLUtils.getChild(commandHandlerEl,"parent"),"cliargument");
					parentValue=MRXMLUtils.getValueXpath(triggerElement,parentXpath, "unknown");
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
						val = MRXMLUtils.getValueXpath(triggerElement, token, null);
					} catch (Exception e) {
						e.printStackTrace();
					}

					if (val != null) {
						cmdArray.add("-" + el.elementText("paramname"));

						// Fix Booleans -- map true/false to Yes/No
						String type = MRXMLUtils.getAttributeValue(el,"type");
						if (type == null) {
							type = "String";
						}
						if (type.compareTo("Boolean") == 0) {
							if (val.compareToIgnoreCase("true") == 0)
								val = "Yes";
							else if (val.compareToIgnoreCase("false") == 0) {
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
						log.addInfo("applyHandler: Could not find value for " + MRXMLUtils.getValue(commandHandlerEl,"cmd","") + "  -> " + token);
					}
				}

				log.addInfo("applyHandler calling ExecuteCLICommand: "+cmdArray.toString());
				int state = MRRequest.STATE_DONE;
				try {
					String result = executeConfigCommand( cmdArray.toArray(new String[cmdArray.size()]));
					log.addInfo("applyHandler called ExecuteCLICommand: result "+ result);
				} catch (Exception e) {					
					state = MRRequest.STATE_EXCEPTION;
					e.printStackTrace();
				}
				Element resultEl = MRXMLUtils.newElement(triggerElement.getName());
				try {
					MRXMLUtils.setValue(resultEl, ""+state);
					MRXMLUtils.addElement(resultdoc.getRootElement(),resultEl);
				} catch (Exception e) {
					e.printStackTrace();
				}
			}
		}

		return false;
	}

	/**
	 * execute device config command
	 * @param cmdArray
	 * @return
	 * @throws Exception
	 */
	public String executeConfigCommand(String[] cmdArray) throws Exception {
		cmdArray[0] = "/V2O/bin/" + cmdArray[0];
		return CommandLineUtils.executeCommand(cmdArray);
	}

	


	protected void handleNotification(String notification) {
		if ((notification == null) || notification.isEmpty()) {
			return;
		}

		try {
			Document notificationDoc = MRXMLUtils.stringToDocument(notification);
			String notificationType = MRXMLUtils.getRootElement(notificationDoc).getName();           
			if (DeviceServerConst.CLI_CONFIG.equals(notificationType)) {
				handleconfigUpdate(notificationDoc);
			}
			else if (DeviceServerConst.CLI_STATUS.equals(notificationType)) {
				handlestatusUpdate(notificationDoc);
			} 

			//TODO handle System notifications and tar notifications (Media Store Related)
			/*else if (DeviceConst.SYSTEM_NOTIFICATION.equals(notificationType)) {
				String eventSource = MRXMLUtils.getValueXpath(notificationDoc, "//EventSource", "");
				if (DeviceConst.MEDIA_FILE_CONVERTER.equals(eventSource) || DeviceConst.MEDIA_FILE_NFS_UPLOAD.equals(eventSource)) {
					handleTarNotification(notificationDoc);
					return ;
				}
				handleSystemNotification(notificationDoc);
			}*/
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

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
			dochandler = MRXMLUtils.loadXMLResource(DeviceServerConst.STATUSHANDLER, log);
			cmd = MRXMLUtils.getValueXpath(notificationDoc, "//CLICommand", null);
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}

		try {
			tableName =MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "']/tablename", null);
			updateXPath =MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "']/triggername", "");
		} catch (Exception e) {
			e.printStackTrace();
		}
		try {
			String locator = null;
			String locatorValue = null;
			locator = MRXMLUtils.getAttributeValueXpath(dochandler, "//handler[cmd='" + cmd + "']/parent", "relativeXpath", null);
			locatorValue = MRXMLUtils.getValueXpath(notificationDoc, 
					"//" + 
							MRXMLUtils.getAttributeValueXpath(dochandler, "//handler[cmd='" + cmd + "']/parent", "name", null), 
					"1");
			if (locator != null) {
				updateXPath += "[" + locator + "=" +  locatorValue + "]";
			}
		} catch (Exception e1) {
			e1.printStackTrace();
		}

		try {
			elementName =MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "']/elementname", "");
		} catch (Exception e) {
			e.printStackTrace();
		}

		if (tableName != null) {
			try {
				el = MRXMLUtils.addElement(tableName);
				Element child=null;
				Element childdata=null;
				List<?> l=null;
				try {
					l = MRXMLUtils.getListXpath(notificationDoc, "//Parameters");
				} catch (Exception e) {
					e.printStackTrace();
				}

				if ((l != null) && !l.isEmpty()) {
					for (int i = 0; i < l.size(); i++) {
						child = MRXMLUtils.addElement(elementName);

						childdata = (Element) l.get(i);
						MRXMLUtils.addElement(child, (Element)childdata.clone());
						MRXMLUtils.addElement(el, child);
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		else {
			Element childdata=null;
			try {
				el = MRXMLUtils.addElement("Childdata");
				childdata = MRXMLUtils.getElementXpath(notificationDoc, "//Parameters");
				MRXMLUtils.addElement(el, (Element)childdata.clone());
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

		try {
			newErrorState = matchErrorState(DeviceServerConst.FAILED, cmd, dochandler, notificationDoc);
			if(newErrorState == null) {
				newErrorState = matchErrorState(DeviceServerConst.ERRORS, cmd, dochandler, notificationDoc);
				if(newErrorState == null) {
					newErrorState = DeviceServerConst.OK;
				}
			}

		} catch (Exception e) {
			e.printStackTrace();
		}

		try {
			deviceXML = FileUtils.readFile(new File(DeviceServerConst.DEVICESTATUSFILE));
			docStatus = MRXMLUtils.stringToDocument(deviceXML);
			MRXMLUtils.replaceContent(docStatus, "//DeviceStatus/" + updateXPath, el);
			if(newErrorState != null) {
				String deviceState = MRXMLUtils.getAttributeValueXpath(docStatus, "//DeviceStatus/" + updateXPath, "State", "");
				if(!deviceState.equals(newErrorState)) {  
					Element element = MRXMLUtils.getElementXpath(docStatus, "//DeviceStatus/" + updateXPath);
					MRXMLUtils.setAttributeValue(element, "State", newErrorState);
					//TODO Send Status updates to Agent / Device Interface to send message out
					//app.handleAgentErrorStatusUpdate(element, newErrorState);
					while((element != null) && (!element.getName().equals("DeviceStatus"))) {
						Element parentEl = MRXMLUtils.getParentElement(element);
						String newParentState = findNewParentState(parentEl);
						String oldParentState = MRXMLUtils.getAttributeValue(parentEl, "State");
						if(newParentState.equals(oldParentState)) {
							break;
						}
						MRXMLUtils.setAttributeValue(parentEl, "State", newParentState);
						//TODO Send Status updates to Agent / Device Interface to send message out
						//app.handleAgentErrorStatusUpdate(parentEl, newParentState);
						element = parentEl;
					}
				}
			}
			FileUtils.writeFile(DeviceServerConst.DEVICESTATUSFILE,MRXMLUtils.documentToString(docStatus));
			//TODO Send Status updates to Agent / Device Interface to send message out
			//app.handleAgentStatusUpdate("//DeviceStatus/" + updateXPath);
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
		String cfgxml;
		Element deviceConfig = null;
		String deviceName=null;

		try {
			el = notificationDoc.getRootElement();

			// Temporary hack by Shanker to discard _auth_ commands
			// Oct 10/13
			String cmd = MRXMLUtils.getValueXpath(el, "//CLICommand", null);

			// Handle failure cases : Shanker
			// Currently just silently discard
			String status = MRXMLUtils.getValueXpath(el, "/CLIConfig/Command/Status", "Failure");

			//System.out.println ("UpdateDeviceConfig :" + status);
			if (status.compareToIgnoreCase("Success") != 0) {
				log.addInfo("updateDeviceConfig Ignoring ERROR " + cmd);
				return;
			}

			if (cmd.compareToIgnoreCase("set_tx_connection_parm") == 0) {
				//System.out.println("updateDeviceConfig Ignoring command " + cmd);
				return;
			}

			dochandler = MRXMLUtils.loadXMLResource(DeviceServerConst.CFGHANDLER, log);
			defaultConfigFile = DeviceServerConst.BASECFGPREFIX + configFile + DeviceServerConst.BASECFGSUFFIX;
			docrefcfg = MRXMLUtils.loadXMLResource(defaultConfigFile, log);
			cfgxml = FileUtils.readFile(new File(configFile));
			docbasecfg = MRXMLUtils.getDocument(cfgxml);
			deviceConfig = MRXMLUtils.getRootElement(docbasecfg);

			if (deviceConfig != null) {
				deviceName = deviceConfig.getName();
			}
			String doDBUpdate=MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/dbupdate", "true");
			String doReset=MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/xmppreset", "false");
			String updateXPath = MRXMLUtils.getValueXpath(dochandler, 
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/triggername", null);
			String action = MRXMLUtils.getValueXpath(dochandler,
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/action", null);
			String updateParentXPath=null;
			if (action.compareToIgnoreCase("ADD")==0) {
				updateParentXPath = MRXMLUtils.getValueXpath(dochandler, 
						"//handler[cmd='" + cmd + "' and device='" + deviceName +"']/tablename", null);
			}


			if (updateXPath == null) {
				log.addInfo("updateDeviceConfig Ignoring Command " + cmd);
				return;
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
				locatorRef = MRXMLUtils.getAttributeValue(eh, "parent", "relativeXpath", null);
				if (locatorRef != null) {
					locatorName = MRXMLUtils.getAttributeValue(eh, "parent", "name", null);
					locatorValue = MRXMLUtils.getValueXpath(el, "//"+ locatorName, null);
					updateXPath += "[" + locatorRef +"=" + locatorValue + "]";
					if (updateParentXPath != null) {
						tableLocatorRef = MRXMLUtils.getAttributeValue(eh, "parent", "tablerelativeXpath", null);
						updateParentXPath += "[" + tableLocatorRef +"='" + locatorValue + "']";
					}
				}
				indexValue = MRXMLUtils.getValueXpath(el, "//Command/Index", null);
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
				FileUtils.writeFile(configFile, MRXMLUtils.documentToString(docbasecfg));
				if (doReset.compareToIgnoreCase("true")==0) {
					// TODO  How to do this: Send A Message to device Interface
					//app.setResetSession(true);
				}
				else {
					//Added by padmaja - to handle Agent Config updated
					if (doDBUpdate.compareToIgnoreCase("true")==0) {
						//TODO  Hand over request to device agent to send message to appserver 
						//app.handleAgentConfigUpdate(updateParentXPath, updateXPath);
					}
				}
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public void updateDirtyChildrenToClean(Document updatedoc, MRLog log){
		try{
			String xPath = "";
			List<?> dirtyList = null;
			xPath = "//*[@" + DeviceAdminConst.DIRTY_TAG + "]";
			dirtyList = MRXMLUtils.getListXpath(updatedoc, xPath);
			for (Iterator<?> i = dirtyList.iterator(); i.hasNext();) {
				Element dirtyEL = (Element) i.next();
				String attrb = MRXMLUtils.getAttributeValue(dirtyEL, DeviceAdminConst.DIRTY_TAG,null);
				if (attrb != null &&   attrb.trim().equalsIgnoreCase("")  ) {
					MRXMLUtils.addAttribute(dirtyEL, DeviceAdminConst.DIRTY_TAG,DeviceAdminConst.CLEAN_FLAG);
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
			deviceConfig = MRXMLUtils.getRootElement(docbasecfg);
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

		xmlsnippet = MRXMLUtils.newElement(cmd);

		Iterator<?> params = el.element("Parameters").elements().iterator();
		Element elparams;

		while (params.hasNext()) {
			elparams = (Element) params.next();
			child = MRXMLUtils.newElement(elparams.elementText("Name"));
			child.addAttribute("value", elparams.elementText("Value"));

			try {
				MRXMLUtils.addElement(xmlsnippet, child);
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
					"//handler[cmd='" + cmd + "' and device='" + deviceName +"']");

			action = apply.element("action").getStringValue();

			log.addInfo("updateConfig :" + cmd + "->" + action);


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
				log.addInfo ("updateConfig :" + action + indexattr);
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
					log.addInfo ("updateConfig :Warning: Name not found " + cmd + ":" + name);
				}
			}

			applyargs = xmlsnippet.elements().iterator();

			while (applyargs.hasNext()) {
				child = (Element) applyargs.next();

				if (child.attributeValue("xpath") == null) {
					log.addInfo ("updateConfig :Warning Ignoring " + cmd + ":" + child.getName());
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
					doc=MRXMLUtils.loadXMLResource(defaultFile, log);
					c1 = MRXMLUtils.getElementXpath(deviceConfig, locatorXpath);
					System.out.println ("locatorXpath" + locatorXpath);
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

			log.addInfo("updateConfig :Updated Command " + cmd);
		} else {
			if (found == false) {
				log.addInfo ("updateConfig :Error Could not handle :" + cmd);
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
			if (MRXMLUtils.setValueXpath(cfgElement, name, value) == null) {
				System.err.println("Error: Failed to update " + name + " for " + cmd);
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
		System.out.println("XPath " + trigger);

		try {
			l = MRXMLUtils.getListXpath(deviceConfig, trigger);
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
		List<?> failedList = MRXMLUtils.getListXpath(parentEl, "./*[@State='" + DeviceServerConst.FAILED + "']");
		if((failedList != null) && (!failedList.isEmpty())) {
			return DeviceServerConst.FAILED;
		}

		List<?> errorList = MRXMLUtils.getListXpath(parentEl, "./*[@State='" + DeviceServerConst.ERRORS + "']");
		if((errorList != null) && (!errorList.isEmpty())) {
			return DeviceServerConst.ERRORS;
		}
		return DeviceServerConst.OK;
	}


	public String matchErrorState(String type, String cmd, Document dochandler, Document notificationDoc) throws Exception{
		List<?> errorList = MRXMLUtils.getListXpath(dochandler, 
				"//handler[cmd='" + cmd + "']/error[@type='" + type + "']" );
		if((errorList != null) && !errorList.isEmpty()) {
			for(Object error:errorList) {
				Element errorEl = (Element)error;
				String errorName = MRXMLUtils.getAttributeValue(errorEl, "name", "");
				String errorValue = MRXMLUtils.getAttributeValue(errorEl, "value", "");
				String errorType = MRXMLUtils.getAttributeValue(errorEl, "type", "");
				String xpath = "//Parameter[@name='" + errorName + "' and .='" + errorValue + "']";
				List<?> l = MRXMLUtils.getListXpath(notificationDoc, xpath);
				if(l != null && !l.isEmpty()) {
					return errorType;
				}
			}
		}
		return null;
	}

	@Override
	public void getDeviceStatus(Element tempDeviceStatusElement) throws Exception {
		Element    statusHandlerDoc = getHandlerDocument(DeviceServerConst.STATUSHANDLER);
		List<?>    statusHandlerChildList = statusHandlerDoc.elements();
		if ((statusHandlerChildList != null) && (statusHandlerChildList.size() != 0)) {
			tempDeviceStatusElement = getDeviceStatus(log, tempDeviceStatusElement, statusHandlerChildList);
		}
	}

	private Element getDeviceStatus(MRLog log, Element deviceStatus, List<?> statusHandlerChildList) {
		//System.out.println("In get device status");
		String cmd = null;
		String triggerName = null;
		Element paramListEL = null;
		List<?> paramList = null;
		Document cmdResultDoc = null;
		Element statusEL = null;
		String elementname =""; 
		for (Iterator<?> handlerIT = statusHandlerChildList.iterator(); handlerIT.hasNext();) {
			Element childEL = (Element) handlerIT.next();
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
					statusEL = MRXMLUtils.getElementXpath(deviceStatus, triggerName);
					// System.out.println("Status xml before update\n" + MRXMLUtils.elementToString(statusEL));
					Element parametersEL = null;
					parametersEL = (Element) (MRXMLUtils.getElementXpath(cmdResultDoc, "//Status").detach());

					//System.out.println("DATA " + MRXMLUtils.elementToString(parametersEL));
					if (parametersEL != null) {
						Element data = MRXMLUtils.getFirstChild(parametersEL);

						if (((data != null) && (statusEL != null))) {

							String parametersElementName = data.getName();
							Element statusParamEL =  MRXMLUtils.getChild(statusEL,parametersElementName);
							if(statusParamEL != null) {
								statusParamEL.detach();
							}	
							MRXMLUtils.addElement(statusEL,(Element)data.detach());	                        
						}
					}
				} catch (Exception e) {
					log.addInfo("Exception occur when get the new status from executed commands in getDeviceStatus() method  : \n" + e.getMessage());
					e.printStackTrace();
				}
			}
		}

		return deviceStatus;
	}

	private Document executeCommandWithParameters(MRLog log, String cmd, List<?> paramList) {
		// StringBuffer command = new StringBuffer();
		ArrayList<String> cmdArray = new ArrayList<String>();

		cmdArray.add(cmd);

		String param = null;
		String paramValue = null;

		for (Iterator<?> objIT = paramList.iterator(); objIT.hasNext();) {
			Element paramEL = (Element) objIT.next();

			try {
				param = paramEL.elementText("paramname");
				paramValue = paramEL.elementText("paramval");

				if ((param != null) && (param.length() != 0)) {
					cmdArray.add("-" + param);
					cmdArray.add(paramValue);
				}
			} catch (Exception exp) {
				log.addInfo("Exception occur when executing the CLI command in executeCommandWithParameters() method  : \n" + exp.getMessage());
				exp.printStackTrace();
			}
		}

		//System.out.println("command  " + command.toString());
		return executeDeviceStatusCommands(cmdArray.toArray(new String[cmdArray.size()]));
	}

	//Execute the CLI command with out parameters list
	private  Document executeCommandWithOutParameters(MRLog log, String cmd) {
		String[] cmdArray = { cmd };

		return executeDeviceStatusCommands(cmdArray);
	}


	private Document executeDeviceStatusCommands(String[] cmdArray) {
		String cmdresult = null;

		try {
			cmdresult = CommandLineUtils.executeCommand(cmdArray);

			int index = cmdresult.indexOf("<");

			if (index != -1) {
				cmdresult = cmdresult.substring(index).trim();
			}
		} catch (Exception e) {
			log.addInfo("Exception occur when executing the CLI command in executeCLICommand() method  : \n" + e.getMessage());
		}

		if ((cmdresult != null) && (cmdresult.length() != 0)) {
			try {
				Document cmdResultdoc = MRXMLUtils.stringToDocument(cmdresult);

				String status = MRXMLUtils.getValueXpath(cmdResultdoc, "//Command/Status", "");

				if (status.equals("Success")) {
					return cmdResultdoc;
				} else {
					return null;
				}
			} catch (Exception e) {
				log.addInfo("Exception occur when executing the ELI command in executeCLICommand() method  : \n" + e.getMessage());
				e.printStackTrace();
			}
		}

		return null;
	}

	//load the xml document 
	private Element getHandlerDocument(String handlerXML) throws Exception {
		Document handlerDoc = null;

		try {
			handlerDoc = MRXMLUtils.loadXMLResource(handlerXML, log);

			if (handlerDoc != null) {
				return handlerDoc.getRootElement();
			}
		} catch (Exception exp) {
			throw new MRException(DeviceAdminExceptionCodes.HANDLER_NOT_FOUND,"Device Config File Not Found");
		}

		return null;
	}

	@Override
	public void resetServer() {
		setResetServer(true);
	}

}
