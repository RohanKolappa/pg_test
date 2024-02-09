package com.ipvs.device.impl;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.net.URLDecoder;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.DeviceApp;
import com.ipvs.mediaroomservice.impl.ConfigDeviceUtils;
import com.ipvs.utils.FileUtils;


public class ConfigListener implements Runnable {
    private volatile static boolean resetCLI = false;
    DeviceApp app = null;
    MRLog log = null;

    public ConfigListener(MRLog log, DeviceApp deviceApp) {
        this.log = log;
        this.app = deviceApp;
    }

    public static boolean isResetCLI() {
        return resetCLI;
    }

    public static void setResetCLI(boolean bresetCLI) {
        resetCLI = bresetCLI;
    }

    private Socket acceptConnection(ServerSocket serverSocket) {
        Socket client = null;

        try {
            client = serverSocket.accept();

            if (client == null) {
                return null;
            }

            client.setSoTimeout(100);
        } catch (Exception exp) {
            //exp.printStackTrace();
            return null;
        }

        log.addInfo("ConfigListener : Accept Client Socket Connection");

        return client;
    }

    private ServerSocket createServerSocketConnection() {
        ServerSocket serverSocket = null;

        try {
            serverSocket = new ServerSocket(DeviceConst.CLI_PORT);
        } catch (IOException e) {
            setResetCLI(true);

            return null;
        }

        try {
            if (serverSocket != null) {
                serverSocket.setSoTimeout(100);
            }
        } catch (SocketException e) {
            //e.printStackTrace();
            return null;
        }

        log.addInfo("ConfigListener : Create ServerSocket");

        return serverSocket;
    }

    private void closeServerSocket(ServerSocket serverSocket) {
        if (serverSocket == null) {
            return;
        }

        try {
            serverSocket.close();
        } catch (IOException e) {
            //e.printStackTrace();
        }

        log.addInfo("ConfigListener : Closing ServerSocket Connection");
    }

    private void closeClientSocket(Socket clientSocket) {
        if (clientSocket == null) {
            return;
        }

        try {
            clientSocket.close();
        } catch (IOException e) {
            //e.printStackTrace();
        }

        log.addInfo("ConfigListener : Closing Client Socket Connection");
    }

    public void run() {
        ServerSocket serverSocket = null;

        serverSocket = createServerSocketConnection();

        Socket clientSocket = null;

        while (!resetCLI) {
            clientSocket = acceptConnection(serverSocket);

            if (clientSocket == null) {
                continue;
            }

            try {
                String notification = getNextcommand(clientSocket);

                if ((notification != null) && !notification.isEmpty()) {
                    handleNotification(notification);
                }
            } catch (Exception e) {
                e.printStackTrace();
            }

            closeClientSocket(clientSocket);
            clientSocket = null;
        }

        closeServerSocket(serverSocket);
        serverSocket = null;
    }

    private void handleNotification(String notification) {
        if ((notification == null) || notification.isEmpty()) {
            return;
        }

        try {
            Document notificationDoc = MRXMLUtils.stringToDocument(notification);
            String notificationType = MRXMLUtils.getRootElement(notificationDoc).getName();           
            if (DeviceConst.CLI_CONFIG.equals(notificationType)) {
                handleconfigUpdate(notificationDoc);
            }
            else if (DeviceConst.CLI_STATUS.equals(notificationType)) {
                    handlestatusUpdate(notificationDoc);
            } else if (DeviceConst.SYSTEM_NOTIFICATION.equals(notificationType)) {
                String eventSource = MRXMLUtils.getValueXpath(notificationDoc, "//EventSource", "");
                if (DeviceConst.MEDIA_FILE_CONVERTER.equals(eventSource) || DeviceConst.MEDIA_FILE_NFS_UPLOAD.equals(eventSource)) {
                    handleTarNotification(notificationDoc);
                    return ;
                }
                handleSystemNotification(notificationDoc);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void handleTarNotification(Document notificationDoc) throws MRException, Exception {
        String eventSource = MRXMLUtils.getValueXpath(notificationDoc, "//EventSource", "");
        String type = MRXMLUtils.getValueXpath(notificationDoc, "//EventType", "");
        XMPPI xmpp = app.getServiceClient().getXMPP(app.getUserJID());
        MRDB db = app.getServiceClient().getMRClientAgent(app.getUserJID()).getDb();
        DeviceAppUtils.handleTarNotification((Element)notificationDoc.getRootElement().detach(), DeviceConst.SYSTEM_NOTIFICATION, eventSource, type, app.getUserJID(), log, xmpp, db);
    }
    private void handleSystemNotification(Document notificationDoc) throws MRException, Exception {
        String eventSource = MRXMLUtils.getValueXpath(notificationDoc, "//EventSource", "");
        String type = MRXMLUtils.getValueXpath(notificationDoc, "//EventType", "");
        XMPPI xmpp = app.getServiceClient().getXMPP(app.getUserJID());
        MRDB db = app.getServiceClient().getMRClientAgent(app.getUserJID()).getDb();
        DeviceAppUtils.handleNotification((Element)notificationDoc.getRootElement().detach(), DeviceConst.SYSTEM_NOTIFICATION, eventSource, type, app.getUserJID(), log, xmpp, db);
    }

    private String getNextcommand(Socket clientSocket) throws Exception {
        BufferedReader reader = null;
        StringBuffer sb = new StringBuffer();

        reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));

       /* if (reader == null) {
            return null;
        } */

        String line = null;

        while ((line = reader.readLine()) != null) {
            if (!clientSocket.isConnected()) {
                return null;
            }

            sb.append(line);
        }

        reader.close();

        return sb.toString();
    }

    private void handleconfigUpdate(Document notificationDoc) throws Exception {
        //Don't add<?XML> content in the log - by Padmaja       
        log.addInfo(MRXMLUtils.documentToStringOmitDeclaration(notificationDoc));
        updateDeviceConfig(notificationDoc);
    }

    private void handlestatusUpdate(Document notificationDoc) throws Exception {
        //Don't add<?XML> content in the log - by Padmaja       
        updateDeviceStatus(notificationDoc);
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
    
    public String findNewParentState(Element parentEl) throws Exception {
    	List<?> failedList = MRXMLUtils.getListXpath(parentEl, "./*[@State='" + DeviceConst.FAILED + "']");
    	if((failedList != null) && (!failedList.isEmpty())) {
    		return DeviceConst.FAILED;
    	}
    	
    	List<?> errorList = MRXMLUtils.getListXpath(parentEl, "./*[@State='" + DeviceConst.ERRORS + "']");
    	if((errorList != null) && (!errorList.isEmpty())) {
          	return DeviceConst.ERRORS;
    	}
    	return DeviceConst.OK;
    }
    
    public void updateDeviceStatus(Document notificationDoc) {
    	

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
            dochandler = MRXMLUtils.loadXMLResource(DeviceConst.STATUSHANDLER, log);
			cmd = MRXMLUtils.getValueXpath(notificationDoc, "//CLICommand", null);
		} catch (Exception e) {
			// TODO Auto-generated catch block
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
			// TODO Auto-generated catch block
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
				// TODO Auto-generated catch block
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
				// TODO Auto-generated catch: block
				e.printStackTrace();
			}
		}
		
		 try {
			 newErrorState = matchErrorState(DeviceConst.FAILED, cmd, dochandler, notificationDoc);
			 if(newErrorState == null) {
				 newErrorState = matchErrorState(DeviceConst.ERRORS, cmd, dochandler, notificationDoc);
				 if(newErrorState == null) {
					 newErrorState = DeviceConst.OK;
				 }
			 }
			 
		 } catch (Exception e) {
				e.printStackTrace();
			}
        
        try {
        	deviceXML = FileUtils.readFile(new File(DeviceConst.DEVICESTATUSFILE));
	    	docStatus = MRXMLUtils.stringToDocument(deviceXML);
	    	MRXMLUtils.replaceContent(docStatus, "//DeviceStatus/" + updateXPath, el);
	    	if(newErrorState != null) {
	    		String deviceState = MRXMLUtils.getAttributeValueXpath(docStatus, "//DeviceStatus/" + updateXPath, "State", "");
	    		if(!deviceState.equals(newErrorState)) {  
	    			Element element = MRXMLUtils.getElementXpath(docStatus, "//DeviceStatus/" + updateXPath);
	    			MRXMLUtils.setAttributeValue(element, "State", newErrorState);
	    			app.handleAgentErrorStatusUpdate(element, newErrorState);
	    			while((element != null) && (!element.getName().equals("DeviceStatus"))) {
	    				Element parentEl = MRXMLUtils.getParentElement(element);
	    				String newParentState = findNewParentState(parentEl);
	    				String oldParentState = MRXMLUtils.getAttributeValue(parentEl, "State");
	    				if(newParentState.equals(oldParentState)) {
	    					break;
	    				}
	    				MRXMLUtils.setAttributeValue(parentEl, "State", newParentState);
	    				app.handleAgentErrorStatusUpdate(parentEl, newParentState);
	    				element = parentEl;
	    			}
	    		}
	    	}
	    	FileUtils.writeFile(DeviceConst.DEVICESTATUSFILE,MRXMLUtils.documentToString(docStatus));
            app.handleAgentStatusUpdate("//DeviceStatus/" + updateXPath);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
    }

    public void updateDeviceConfig(Document notificationDoc) {
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

            dochandler = MRXMLUtils.loadXMLResource(DeviceConst.CFGHANDLER, log);
            defaultConfigFile = DeviceConst.BASECFGPREFIX + app.getDefaultConfigFile() + DeviceConst.BASECFGSUFFIX;
            docrefcfg = MRXMLUtils.loadXMLResource(defaultConfigFile, log);
            cfgxml = FileUtils.readFile(new File(app.getLocalConfigFile()));
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
            ConfigDeviceUtils.updateDirtyChildrenToClean(docbasecfg, log);

            if (el != null) {
                updateConfig(el, dochandler, docbasecfg, docrefcfg);
                FileUtils.writeFile(app.getLocalConfigFile(), MRXMLUtils.documentToString(docbasecfg));
                if (doReset.compareToIgnoreCase("true")==0) {
                    app.setResetSession(true);
                }
                else {
                	//Added by padmaja - to handle Agent Config updated
                	if (doDBUpdate.compareToIgnoreCase("true")==0) {
                		app.handleAgentConfigUpdate(updateParentXPath, updateXPath);
                	}
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
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
					// TODO Auto-generated catch block
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
                for (Object obj : listChild) {
                	Element object = (Element) obj;
                    setXpath_decendents(object, cfgElement, cmd);
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
    @SuppressWarnings("unchecked")
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
        List<Element> listChild =(List<Element> ) child.elements();
        for (Element object : listChild) {
            setXpath_decendents(object, cfgElement, cmd);
        }
    }
    private static void deleteEntry(Element deviceConfig, String trigger) {
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

}
