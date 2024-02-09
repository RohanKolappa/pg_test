package com.ipvs.common.service;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;


public class XAuthServerAgent implements ServiceAgentI {
    //private XMPPI xmpp;	
    protected MRLog log;
    //private MRResourceFactoryI resourceFactory;
    //private ServiceAgentFactoryI profileFactory;
    protected String userJID;
    protected MRDB db =null;
    /*XAuth Table Structure
	 * <xAuthRequestTable>
	 * 	  <xAuthRequest>
	 * 	   --requestReceivedFromClient--	
	 * 	   <state />
	 * 		<startTime />
	 * 	   <data />	
	 *    </xAuthRequest>
	 * </xAuthRequestTable>
	 * 
	 */
    
    public XAuthServerAgent() {
    	try {
        	xAuthRequestTable = MRXMLUtils.elementToDocument(MRXMLUtils.newElement("xAuthRequestTable"));
        }catch(Exception e) {
        	e.printStackTrace();
        }
    }
	
    private Document xAuthRequestTable = null;
    Hashtable<String, XAuthServerAgentObserverI> observers = new Hashtable<String, XAuthServerAgentObserverI>();

    public void addXAuthEntry(Document xAuthRequestDoc, int state) {
    	
    	Element startTimeEL = MRXMLUtils.newElement("startTime");
    	startTimeEL.setText("0");
    	Element stateEL = MRXMLUtils.newElement("state");
    	stateEL.setText(String.valueOf(state));
    	Element dataEL = MRXMLUtils.newElement("data");
		Element xAuthRequest = MRXMLUtils.newElement ("xAuthRequest");
		MRXMLUtils.addElement(xAuthRequest,(Element)dataEL.detach());
		MRXMLUtils.addElement(xAuthRequest,(Element)stateEL.detach());
		MRXMLUtils.addElement(xAuthRequest,(Element)startTimeEL.detach());
		MRXMLUtils.addElement(xAuthRequest,(Element)xAuthRequestDoc.getRootElement().detach());
		MRXMLUtils.addElement(xAuthRequestTable.getRootElement(),xAuthRequest);
    }
    
    public Document getXAuthEntry(String streamNID) {
		try{
			String xpath = "//xAuthRequest[.//streamNID='" + streamNID + "']";
			Element xAuthRequestMessage = MRXMLUtils.getElementXpath(xAuthRequestTable, xpath);
			if(xAuthRequestMessage == null) {
				return null;
			}
			return MRXMLUtils.elementToDocument(xAuthRequestMessage);
		} catch(Exception e) {
			e.printStackTrace();
			return null;
		}
	}
    
    public String getRoomNID(String destAgentJID) {
		try{
			String xpath = "//xAuthRequest[.//destAgentJID='" + destAgentJID + "']";
			Element xAuthRequestMessage = MRXMLUtils.getElementXpath(xAuthRequestTable, xpath);
			
			if(xAuthRequestMessage!=null) {
				return MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(xAuthRequestMessage),"//roomNID","");
			}
			return "";
		} catch(Exception e) {
			e.printStackTrace();
			return null;
		}
	}

    @SuppressWarnings("unchecked")
    public List<Element> getXAuthEntryList(String roomNID) {
		try{
			if(roomNID==null) {
				List<Element> xAuthRequestList = xAuthRequestTable.getRootElement().elements("xAuthRequest");
				return xAuthRequestList;
			}
			String xpath = "//xAuthRequest[.//roomNID='" + roomNID + "']";
			List<Element> xAuthRequestList = (List<Element>)MRXMLUtils.getListXpath(xAuthRequestTable,xpath);
			return xAuthRequestList;
		} catch(Exception e) {
			e.printStackTrace();
			return null;
		}
	}
    
    public void deleteXAuthEntry(Element xAuthRequestMessage) {
    	if(xAuthRequestMessage!=null) {
    		xAuthRequestMessage.getParent().remove(xAuthRequestMessage);
    	}
    	
    }
    @SuppressWarnings("unchecked")
	public void deleteXAuthEntriesForRoom(String roomNID) {
		try {
			String xpath = "//xAuthRequest[.//roomNID='" + roomNID + "']";
			List<Element> xAuthRequestList = (List<Element>)MRXMLUtils.getListXpath(xAuthRequestTable,xpath);
			if(xAuthRequestList!=null) {
				for(int i=0;i<xAuthRequestList.size();i++) {
					if(xAuthRequestList.get(i)!=null) {
					    MRXMLUtils.getParentElement(xAuthRequestList.get(i)).remove(xAuthRequestList.get(i));
					}	
				}
			}	
		}catch(Exception e){
			e.printStackTrace();
		}
		
	}
	
    
    public void updateState(int state, String streamNID, String data) {
    	try{
			String xpath = "//xAuthRequest[.//streamNID='" + streamNID + "']";
			Element xAuthRequestMessage = MRXMLUtils.getElementXpath(xAuthRequestTable, xpath);
			if(xAuthRequestMessage ==  null) {
				return;
			}
			else {
				MRXMLUtils.setValueXpath(xAuthRequestMessage, "state", String.valueOf(state));
				if(data!=null) {
					MRXMLUtils.setValueXpath(xAuthRequestMessage, "data", data);
				}
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
    }
    
    public void setStartTime(String streamNID, String startTime) {
    	try{
			String xpath = "//xAuthRequest[.//streamNID='" + streamNID + "']";
			Element xAuthRequestMessage = MRXMLUtils.getElementXpath(xAuthRequestTable, xpath);
			if(xAuthRequestMessage ==  null) {
				return;
			}
			else {
				MRXMLUtils.setValueXpath(xAuthRequestMessage, "startTime", startTime);
			}
		} catch(Exception e) {
			e.printStackTrace();
		}
    }
    
    public boolean handleEvent(MRClient client, Document doc, Element event)
        throws Exception {
        // TODO Auto-generated method stub
        return false;
    }

    public void addAgentObserver(String key, XAuthServerAgentObserverI observer) {
        System.out.println(this.hashCode() + "Adding observer key=" + key);
        observers.put(key, observer);
    }

    public void removeAgentObserver(String key) {
        observers.remove(key);
    }

    public String handleMessage(MRClient client, String fromJID, String message)
        throws Exception {
        Iterator<String> i = this.observers.keySet().iterator();
        while (i.hasNext()) {
        	XAuthServerAgentObserverI observer = this.observers.get(i.next());
            try {
            	Document xAuthRequestDoc = MRXMLUtils.stringToDocument(message);
            	addXAuthEntry(xAuthRequestDoc, MediaRoomServiceConst.WAITING_FOR_XAUTH);
                observer.handleXAuthRequest(message);
            } catch (Exception e) {
            	e.printStackTrace();
            }
        }
        return message;
    }
 
    public void handleXAuthResponse(Document message, MRLog log, XMPPI xmpp) throws Exception {
    	//String fromJID = null;
    	log.addInfo(":XAuthServerAgent@handleXAuthResponse", MRLog.INFO,MRLog.NOTIFY);
    	String result = null;
    	String response = null;
    	String id = MRXMLUtils.getValueXpath(message,"//requestID", "");
    	int exceptionCode = 0;
    	Document xauthRequestDoc = getXAuthEntry(id);
    	if(xauthRequestDoc == null) {
    		return;
    	}
    	String state = MRXMLUtils.getValueXpath(xauthRequestDoc, "//xAuthRequest/state","");
    	try {
    		if(Integer.valueOf(state) != MediaRoomServiceConst.WAITING_FOR_XAUTH) {
    			log.addInfo(":State of stream is not WAITING_FOR_XAUTH", MRLog.INFO,MRLog.NOTIFY);
    			return;
    		}
    	}
    	catch(Exception ex) {
    		ex.printStackTrace();
    	}
    	int requestState = MRRequest.STATE_DONE;
    	//int streamState = MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED;
    	result = MRXMLUtils.getValueXpath(message, "//Result", "unknown");
		if (result.compareToIgnoreCase("Allow") == 0) {
			response = "confirmed";
			String streamNID = MRXMLUtils.getValueXpath(xauthRequestDoc, "//streamNID", "");
			updateState(MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED ,streamNID,String.valueOf(xmpp.getCurrentTimeMillis()));
		}
		else {
			response = "";
			requestState = MRRequest.STATE_EXCEPTION;
			String streamNID = MRXMLUtils.getValueXpath(xauthRequestDoc, "//streamNID", "");
			updateState(MediaRoomServiceConst.DENIED_XAUTH ,streamNID,String.valueOf(xmpp.getCurrentTimeMillis()));
			exceptionCode = MRExceptionCodes.XAUTH_DENIED;
		}
		log.addInfo(":Sent XAuthResponse", MRLog.INFO,MRLog.NOTIFY);
		sendXAuthResponse(xauthRequestDoc, response, requestState, exceptionCode, log, xmpp);
    }
    
   
    public void handleRevokeXAuth(String destAgentJID, MRLog log, XMPPI xmpp) throws Exception {
    	try{
    		log.addInfo(":XAuthServerAgent@handleRevokeXAuth", MRLog.INFO,MRLog.NOTIFY);
			String xpath = "//xAuthRequest[.//destAgentJID='" + destAgentJID + "']";
			Element xAuthRequestMessage = MRXMLUtils.getElementXpath(xAuthRequestTable, xpath);
			if(xAuthRequestMessage == null) {
				return;
			}
			log.addInfo(":Send XAuthResponse", MRLog.INFO,MRLog.NOTIFY);
			sendXAuthResponse(MRXMLUtils.elementToDocument(xAuthRequestMessage), "", MRRequest.STATE_EXCEPTION, MRExceptionCodes.XAUTH_REVOKED, log, xmpp);
			String streamNID = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(xAuthRequestMessage), "//streamNID", "");
			updateState(MediaRoomServiceConst.REVOKED_XAUTH ,streamNID,String.valueOf(xmpp.getCurrentTimeMillis()));
			
		} catch(Exception e) {
			e.printStackTrace();
		}
    }
    
    public void sendXAuthResponse(Document messageDoc, String xauthToken, int requestState, int exceptionCode,  MRLog log, XMPPI xmpp) throws Exception {
    	try{
			
			if(messageDoc!=null) {
	    		Element data = ServiceAgentUtils.cloneDataElement(messageDoc);
	    		String requestNID = MRXMLUtils.getValueXpath(messageDoc, "//requestNID","");
	    		MRXMLUtils.addElement(data, MRXMLUtils.tagToElement("<xAuthtoken>" + xauthToken + "</xAuthtoken>"));
	    		String logdata = "";
	    		if(requestState == MRRequest.STATE_EXCEPTION) {
	    			Exception ex = new MRException(exceptionCode, requestNID);
    				logdata = ex.toString();
	    		}		
	    		Document requestDoc = MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(messageDoc, "//Request"));
	    		String requestName = MRXMLUtils.getValueXpath(requestDoc, "//requestname", ""); 
	    		String info = "XAuthServerAgent: Sending xAuthResponse from " + MRServiceConst.XAUTHSERVERAGENT + " to " + MediaRoomServiceConst.APPSERVERAGENT;
	        	Element messageElement = MRXMLUtils.getElementXpath(messageDoc, "//message");
	        	String fromJID = MRXMLUtils.getAttributeValue(messageElement,"from");
	            ServiceAgentUtils.sendAgentResponseMessage(xmpp, log, null, MRServiceConst.XAUTHSERVERAGENT, fromJID, MediaRoomServiceConst.APPSERVERAGENT, ServiceClient.MEDIAROOMSERVICE, requestName, MRXMLUtils.elementToString(data), info,requestState, logdata, requestNID,
	                    MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.USER_JID, ""), MRXMLUtils.getValueXpath(requestDoc, "//"+MRConst.CLIENT_DATA, ""));
				
			}
			
		} catch(Exception e) {
			e.printStackTrace();
		}
    	
    }
    
    
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID,Document setServiceDomainResponseDoc) {
        //this.xmpp = xmpp;
        this.log = log;
        //this.resourceFactory = resourceFactory;
        //this.profileFactory = profileFactory;
        this.db=db;
        this.userJID = userJID;
        
    }

    public boolean handleRoomUnavailable(String roomNID)
        throws Exception {
        Iterator<String> count = this.observers.keySet().iterator();
        while (count.hasNext()) {
        	XAuthServerAgentObserverI observer = this.observers.get(count.next());
            try {
                List<Element>streamList = getXAuthEntryList(roomNID);
                if(streamList!=null) {
                	for(int i=0;i<streamList.size();i++) {
                		Document streamDoc = MRXMLUtils.elementToDocument(streamList.get(i));
                		if(streamDoc!=null) {
                			Document streamDataDoc = MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(streamDoc, "//streamDataDoc"));
                			if(streamDataDoc!=null) {
                				int streamState = Integer.valueOf(MRXMLUtils.getValueXpath(streamDoc, "//state", ""));
            	            	if(streamState == MediaRoomServiceConst.STREAM_STARTED) {
            	            		observer.handleXAuthDeleted(streamDoc);
            	            	}
            	            	else {
            	            		observer.handleXAuthCancelled(streamDoc);
            	            	}
                			}
                		}
                	}	
                }
                deleteXAuthEntriesForRoom(roomNID);
                return true;
            } catch (Exception e) {
            	e.printStackTrace();
            }
        }
        return false;
    }

	@Override
	public void reset() throws Exception {
		// TODO Auto-generated method stub
		
	}
}
