package com.ipvs.mediaroomservice.impl;

import java.io.File;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.FileUtils;
   
public class ConfigDeviceUtils {
    public static final String DEVICESTATUS = "DeviceStatus";
    public static final String DEVICESTATUSFILE = "/tmp/Device.xml";
    public static final String STREAMINGSERVICESTATUS = "StreamingServiceStatus";
    public static final String STATUSHANDLERXML = "/com/ipvs/xsd/xml/statushandler.xml";
    public static final String COMMANDHANDLERXML = "/com/ipvs/xsd/xml/commandhandler.xml";
    public static final String GET_STREAMING_STATUS_CLI_CMD = "get_streaming_status";

	public static final String DIRTY_TAG="Dirty";
	public static final String CLEAN_FLAG="CLEAN";
	public static final String MODIFY_FLAG="MODIFY";
	public static final String ADD_FLAG="ADD";
	public static final String INIT_FLAG="INIT";
	public static final String DELETE_FLAG="DELETE";
	public static final String ERROR_FLAG="ERROR";
	
	//private String calledFrom="";
	public ConfigDeviceUtils(String calledFrom, MRLog log) {
		//this.calledFrom = calledFrom;
		log.addInfo("/////////ConfigDeviceUtils calledFrom /////////" + calledFrom);
	}
 	//TODO: code form DeviceApp and ConfigStreamAgent to be combined and moved to a thread.
	public static  Document handleConfig(Document docconfig, String handlerFile, MRLog log) {
   		Document dochandler = null;
        Element el = null;
        //List<?> l = null;
        int i = 0;

        try {
            dochandler = MRXMLUtils.loadXMLResource(handlerFile, log);
        } catch (Exception e) {
            StackTraceElement[] st = e.getStackTrace();

            for (i = 0; i < st.length; i++) {
                System.out.println(st[i].toString());

                if (i > 5) {
                    break;
                }
            }

            System.out.println(e.toString());
        }
        if(dochandler == null) {
        	return null;
        }
        Iterator<?> iter = dochandler.getRootElement().elements().iterator();
    	Document resultdoc= null;
		try {
			resultdoc= MRXMLUtils.stringToDocument("<Result/>");
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
        while (iter.hasNext()) {
            el = (Element) iter.next();
            applyHandler(docconfig, el, log,resultdoc);
        }
        return resultdoc;
    }
	
	public static boolean applyHandler(Document doc, Element h, MRLog log,Document resultdoc) {
		Element el = null;
		String trigger = h.elementText("triggername");
		String expectResponse = h.elementText("expectResponse");
		boolean bsendClientData = false;
	
		
		List<?> l = null;

		String operation =MRXMLUtils.getValue(h,"action","");
		String dirtyFlag ="Unknown";

		String listIndex=null;


		ArrayList<String> cmdArray = new ArrayList<String>();

		if ((expectResponse != null) && (expectResponse.compareToIgnoreCase("yes") == 0)) {
			bsendClientData = true;
		}

		//System.out.println("Finding :" + trigger);
		try {
			l = MRXMLUtils.getListXpath(doc, trigger);
		} catch (Exception e) {
			//log.addInfo("applyHandler getListXpath Error  "+e.getMessage());
			e.printStackTrace();
		}

		if (l != null && !l.isEmpty()) {

			for (int i = 0; i < l.size(); i++) {
				// Do nothing if the flag does not match the 
				// action in the command except for startup : Shanker 11/12008

				Element c = (Element) l.get(i);
				try {
					dirtyFlag = MRXMLUtils.getAttributeValue(c,"Dirty");
				}
				catch (Exception e) {
					dirtyFlag=operation;
				}
				
				
				
				if (dirtyFlag == null){
					dirtyFlag = INIT_FLAG;
				}
				if (dirtyFlag.compareToIgnoreCase(INIT_FLAG)==0) {
					if (operation.compareToIgnoreCase("ADD")==0) {
						dirtyFlag = operation;
					}
					else if (operation.compareToIgnoreCase("MODIFY")==0) {
						dirtyFlag = operation;
					}
				}
				if (dirtyFlag.compareToIgnoreCase(operation)!=0) {
					continue;
				}

				//StringBuffer sb = new StringBuffer();
				//sb.append("echo ");
				cmdArray.clear();
				cmdArray.add(MRXMLUtils.getValue(h,"cmd",""));

				if (bsendClientData) {
					cmdArray.add("-clientData");
					cmdArray.add("001");
				}

				if (operation.compareToIgnoreCase("DELETE")==0) {
					try {
						listIndex =MRXMLUtils.getAttributeValue( c,"ListIndex");
					}
					catch (Exception e) {
						listIndex="0";
					}
					cmdArray.add(listIndex);
				}
				if (operation.compareToIgnoreCase("UPDATELIST")==0) {
					try {
						listIndex = MRXMLUtils.getAttributeValue(c,"ListIndex");
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
					parentXpath=MRXMLUtils.getAttributeValue(MRXMLUtils.getChild(h,"parent"),"relativeXpath");
					parentArg=MRXMLUtils.getAttributeValue(MRXMLUtils.getChild(h,"parent"),"cliargument");
                    parentValue=MRXMLUtils.getValueXpath(c,parentXpath, "unknown");
					cmdArray.add("-" + parentArg);
					cmdArray.add(parentValue);
				}
				catch (Exception e) {
					// Do nothing some commands do not have parents
				}

				Iterator<?> iter = h.element("paramtable").elements().iterator();

				while (iter.hasNext()) {
					el = (Element) iter.next();

					String token = el.elementText("paramval");
					String val = null;

					try {
						val = MRXMLUtils.getValueXpath(c, token, null);
					} catch (Exception e) {
						e.printStackTrace();
					}

					if (val != null) {
						// No longer need this??
	     				// if (val.contains(" ")) {
						//   val = val.replace(' ', '_');
						// }
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
								// TODO Auto-generated catch block
							}
						}
						// Add the value to the command array
						cmdArray.add(val);
					} else {
						log.addInfo("applyHandler: Could not find value for " + MRXMLUtils.getValue(h,"cmd","") + "  -> " + token);
					}
				}

				log.addInfo("applyHandler calling ExecuteCLICommand: "+cmdArray.toString());
				int state = MRRequest.STATE_DONE;
				try {
					
					String result = MediaRoomServiceUtils.executeCLICommand( cmdArray.toArray(new String[cmdArray.size()]));
					log.addInfo("applyHandler called ExecuteCLICommand: result "+ result);
				} catch (Exception e) {					
					state = MRRequest.STATE_EXCEPTION;
					e.printStackTrace();
				}
				Element resultEl = MRXMLUtils.newElement(c.getName());
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
	  
	 public static  void updateDirtyChildrenToClean(Document updatedoc, MRLog log){
	    	try{
	    		String xPath = "";
				List<?> dirtyList = null;
				// xPath="//*[@Dirty]";
				xPath = "//*[@" + DIRTY_TAG + "]";
				dirtyList = MRXMLUtils.getListXpath(updatedoc, xPath);
				for (Iterator<?> i = dirtyList.iterator(); i.hasNext();) {
					Element dirtyEL = (Element) i.next();
					String attrb = MRXMLUtils.getAttributeValue(dirtyEL, DIRTY_TAG,null);
					if (attrb != null &&   attrb.trim().equalsIgnoreCase("")  ) {
						//log.addInfo("updateDirtyChildrenToClean dirtyEL : " + dirtyEL.getName()+ " Dirty=" + attrb);
						MRXMLUtils.addAttribute(dirtyEL, DIRTY_TAG,CLEAN_FLAG);
					} else{
						//log.addInfo("updateDirtyChildrenToClean donothing dirtyEL : " + dirtyEL.getName()+ " Dirty=" + attrb);
					}
				}
			}catch(Exception ex){
				ex.printStackTrace();
			}
	    }	 
	  public static  Document findDirtyChildren(Document updatedoc, MRLog log){
	        Document docModify =null;
    		String xPath = "";
			List<?> cleanList = null;
	    	try{
				docModify = MRXMLUtils.getDocument("<DeviceModifications/>");
				xPath = "//DeviceConfig";
				MRXMLUtils.addElement(docModify.getRootElement(),
						(Element)updatedoc.getRootElement().clone());
				xPath="//*[@"+DIRTY_TAG+"='"+CLEAN_FLAG+"'" + 
						"or @"+DIRTY_TAG+"=''" +"]";
				cleanList=MRXMLUtils.getListXpath(docModify, xPath);
	    		for (Iterator<?> i = cleanList.iterator(); i.hasNext();) {
	    			Element cleanEL= (Element)i.next();
     				cleanEL.detach();
	    		}
			}catch(Exception ex){
				ex.printStackTrace();
			}
			return docModify;
	    }
	    public static void updateStatus(MRLog log) throws Exception {
	        Element deviceStatus = null; 
	        Element statusHandlerDoc = null;
	        
	        List<?> statusHandlerChildList = null;
	        Element newDeviceStatusEL = null;
	        
	    	String deviceXML = FileUtils.readFile(new File(DEVICESTATUSFILE));
	    	Document doc = MRXMLUtils.stringToDocument(deviceXML);
	    	deviceStatus = MRXMLUtils.getElementXpath(doc,"//DeviceStatus");

	        try {
	                statusHandlerDoc = getHandlerDocument(log, STATUSHANDLERXML);
	                statusHandlerChildList = statusHandlerDoc.elements();

	                if ((statusHandlerChildList != null) && (statusHandlerChildList.size() != 0)) {
	                    newDeviceStatusEL = getDeviceStatus(log, deviceStatus, statusHandlerChildList);
	                    if (newDeviceStatusEL != null) {
	                        updateDeviceStatusLocally (log, newDeviceStatusEL, DEVICESTATUSFILE);
	                    }
	                }
	        } catch (Exception exp) {
	            exp.printStackTrace();
	        }
	    }
	    private static void updateDeviceStatusLocally(MRLog log,
				Element deviceStatusEL, String deviceStatusFile) throws Exception {
	    	String deviceXML = FileUtils.readFile(new File(deviceStatusFile));
	    	Document doc = MRXMLUtils.stringToDocument(deviceXML);
	    	Element statusEL = MRXMLUtils.getElementXpath(doc,"//DeviceStatus");
	    	MRXMLUtils.replaceContent(statusEL, deviceStatusEL);
	    	FileUtils.writeFile(deviceStatusFile,MRXMLUtils.documentToString(doc));
	    return;
			
		}
		//run the CLI command and the get the status from device and update it with command result  
	    private static Element getDeviceStatus(MRLog log, Element deviceStatus, List<?> statusHandlerChildList) {
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

	    //execute the CLI command with parameters list
	    private static Document executeCommandWithParameters(MRLog log, String cmd, List<?> paramList) {
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
	        return ServiceAgentUtils.executeCLICommand(cmdArray.toArray(new String[cmdArray.size()]), log);
	    }

	    //Execute the CLI command with out parameters list
	    private static Document executeCommandWithOutParameters(MRLog log, String cmd) {
	        String[] cmdArray = { cmd };

	        return ServiceAgentUtils.executeCLICommand(cmdArray, log);
	    }
	    //load the xml document 
	    private static Element getHandlerDocument(MRLog log, String handlerXML) {
	        Document handlerDoc = null;

	        try {
	            handlerDoc = MRXMLUtils.loadXMLResource(handlerXML, log);

	            if (handlerDoc != null) {
	                return handlerDoc.getRootElement();
	            }
	        } catch (Exception exp) {
	            log.addInfo("Exception occur when getting the handler XML getHandlerDocument() method  : \n" + exp.getMessage());
	            exp.printStackTrace();
	        }

	        return null;
	    }
		public static void executeDeviceCommand(Document cmdDocument, MRLog log) {
	        Element commandHandlerDoc = null;
	        String triggerName=null;
	        @SuppressWarnings("unused")
			Element paramListEL=null;
	        String cmd=null;
	        //@SuppressWarnings("unused")
			Element deviceCommandElement=null;

	        List<?> commandHandlerChildList = null;
	        //Match the command(s) from the message
	        try {
	                commandHandlerDoc = getHandlerDocument(log, COMMANDHANDLERXML);
	                commandHandlerChildList = commandHandlerDoc.elements();
	    	        for (Iterator<?> handlerIT = commandHandlerChildList.iterator(); handlerIT.hasNext();) {
	    	            Element childEL = (Element) handlerIT.next();
	    	            cmd = childEL.elementText("cmd");
	    	            triggerName = childEL.elementText("triggername");
	    	            paramListEL = childEL.element("paramtable");
	    	            try {
	    	                    deviceCommandElement = MRXMLUtils.getElementXpath(cmdDocument, "//" + triggerName);
	    	                } catch (Exception e) {
	    	                }
	    	                if (deviceCommandElement != null) {
	    	                	break;
	    	                }
	    	            }
	    	        if (deviceCommandElement != null) {
	    	        	executeCommandWithOutParameters(log,cmd);
	                    log.addInfo("Match Succceeded " + triggerName + " Executing " + cmd);
	    	        }
	    	        else {
	                    log.addInfo("Did not match " + triggerName);
	    	        }
	        } catch (Exception exp) {
	            exp.printStackTrace();
	        }
	        
		}
		public static void initDirtyChildrenFlag(Document docconfig, MRLog log) {
	    	try{
	    		String xPath = "";
				List<?> dirtyList = null;
				// xPath="//*[@Dirty]";
				xPath = "//*[@" + DIRTY_TAG + "]";
				dirtyList = MRXMLUtils.getListXpath(docconfig, xPath);
				for (Iterator<?> i = dirtyList.iterator(); i.hasNext();) {
					Element dirtyEL = (Element) i.next();
					String attrb = MRXMLUtils.getAttributeValue(dirtyEL, DIRTY_TAG,null);
					if (attrb != null) {
						//log.addInfo("updateDirtyChildrenToClean dirtyEL : " + dirtyEL.getName()+ " Dirty=" + attrb);
						MRXMLUtils.addAttribute(dirtyEL, DIRTY_TAG,INIT_FLAG);
					} else{
					}
				}
			}catch(Exception ex){
				ex.printStackTrace();
			}

		}
}
