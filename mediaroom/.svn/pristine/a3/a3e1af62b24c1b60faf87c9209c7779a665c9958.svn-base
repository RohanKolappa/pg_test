package com.ipvs.mediaroomservice.impl;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class MediaRoom {
	
	Document roomdoc;
	String type;
	ArrayList<String> unavailableAssetResourceNIDList = new ArrayList<String>();
	
	public MediaRoom(Document roomdoc, String type) {
		this.roomdoc = roomdoc;
		this.type = type;
	}
	
	public String getType() {
		return this.type;
	}
	
	public void addUnavailableAssetResourceNID(String assetResourceNID) {
		if(!"".equals(assetResourceNID) && !unavailableAssetResourceNIDList.contains(assetResourceNID))
			unavailableAssetResourceNIDList.add(assetResourceNID);
	}
	
	public void removeUnavailableAssetResourceNID(String assetResourceNID) {
		if(unavailableAssetResourceNIDList.contains(assetResourceNID))
			unavailableAssetResourceNIDList.remove(assetResourceNID);
	}
	
	public ArrayList<String> getUnavailableAssetResourceNIDList() {
		return unavailableAssetResourceNIDList;
	}
	
	public synchronized Document getRoomDoc() throws Exception {
		return (Document)roomdoc.clone();
	}

    private synchronized Element readElementInternalByXpath(MRRequestI request, MRLog log, MRDB db, 
    		String xpath)
    throws Exception {
    	Element element = MRXMLUtils.getElementXpath(roomdoc, xpath);
    	return element;
    }

    private Element readElementInternal(MRRequestI request, MRLog log, MRDB db, String NID)
    throws Exception {
    	String xpath = "//*[@NID='" + NID + "']";
    	return this.readElementInternalByXpath(request, log, db, xpath);
    }

    public synchronized String getObjectData(MRRequestI request, MRDB db, MRLog log, String NID)
    throws Exception {
    	Element element = this.readElementInternal(request, log, db, NID);
    	if(element == null)
    		return null;
		Element dataElement = (Element)element.element("data").clone();
		Document doc = MRXMLUtils.elementToDocument(dataElement);
		return MRXMLUtils.documentToString(doc);
    }

    public synchronized void setObjectData(MRRequestI request, MRDB db, MRLog log, String NID, String data)
    throws Exception {
    	// MRXMLDBUtils.setObjectData(request, db, log, NID, data);
    	Element element = this.readElementInternal(request, log, db, NID);
    	if(element == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "<NID>" + NID + "</NID>");
		Element dataElement = (Element)(MRXMLUtils.stringToDocument(data).getRootElement()).detach();
		element.element("data").detach();
		element.add(dataElement);
    }

    public synchronized void setObjectData(MRRequestI request, MRDB db, MRLog log, String NID, Element dataElement)
    throws Exception {
    	Element element = this.readElementInternal(request, log, db, NID);
    	if(element == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "<NID>" + NID + "</NID>");
		element.element("data").detach();
		element.add((Element)dataElement.clone());
    }


    public synchronized boolean objectExists(MRRequestI request, MRDB db, MRLog log, String NID)
    throws Exception {
    	Element element = this.readElementInternal(request, log, db, NID);
		return (element != null);
    }

    
    public synchronized Document readElement(MRRequestI request, MRLog log, MRDB db, String NID)
    throws Exception {
    	Element element = this.readElementInternal(request, log, db, NID);
    	if(element == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "<NID>" + NID + "</NID>");
		return MRXMLUtils.elementToDocument(element);
    }

    public synchronized Document readElementByXpath(MRRequestI request, MRLog log, MRDB db, String xpath)
    throws Exception {
    	Element element = this.readElementInternalByXpath(request, log, db, xpath);
    	if(element == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "<xpath>" + xpath + "</xpath>");
		return MRXMLUtils.elementToDocument(element);
    }

    public synchronized String addMediaSourceElement(MRRequestI request, MRLog log, MRDB db, 
    		Element element, String trackNID) throws Exception {
XMPPTransaction.startBlock("addMediaSourceElement");
    	Element trackElement = this.readElementInternal(request, log, db, trackNID);
    	trackElement.element("mediasourcelist").add((Element)element.clone());
XMPPTransaction.endBlock("addMediaSourceElement");
    	return MRXMLUtils.getAttributeValue(element,"NID");
    }

    public synchronized String addMediaDestElement(MRRequestI request, MRLog log, MRDB db, 
    		Element element, String trackNID) throws Exception {
    	Element trackElement = this.readElementInternal(request, log, db, trackNID);
    	MRXMLUtils.addElement(MRXMLUtils.getChild(trackElement,"mediadestlist") ,(Element)element.clone());
    	return MRXMLUtils.getAttributeValue(element,"NID");
    }
    public synchronized String addMediaRelayElement(MRRequestI request, MRLog log, MRDB db, 
    		Element element, String trackNID) throws Exception {
    	Element trackElement = this.readElementInternal(request, log, db, trackNID);
    	MRXMLUtils.addElement(MRXMLUtils.getChild(trackElement,"mediarelaylist"),(Element)element.clone());
    	return MRXMLUtils.getAttributeValue(element,"NID");
    }
    public synchronized String addStreamElement(MRRequestI request, MRLog log, MRDB db, 
    		Element element, String mediaSourceNID) throws Exception {
XMPPTransaction.startBlock("addStreamlement");
    	Element mediaSourceElement = this.readElementInternal(request, log, db, mediaSourceNID);
    	Element trackElement = mediaSourceElement.getParent().getParent();
    	MRXMLUtils.addElement(MRXMLUtils.getChild(trackElement,"streamlist"),(Element)element.clone());
XMPPTransaction.endBlock("addStreamlement");
    	return MRXMLUtils.getAttributeValue(element,"NID");
    }
    


    public synchronized void updateStreamContactJID(String streamXPath, String streamDataElementName, String contactJID) throws Exception {
		List<?> streamList = MRXMLUtils.getListXpath(roomdoc, streamXPath);
		for(Iterator<?> i = streamList.iterator(); i.hasNext();) {
			Element stream = (Element)i.next();
			// System.out.println("RoomDB:updateContactJID stream=" + MRXMLUtils.elementToString(stream));
			stream.element(streamDataElementName).setText(contactJID);
		}
		// System.out.println(MRXMLUtils.documentToString(roomdoc));
    }

    public synchronized List<Element> updateMediaSourceContactJID(String mediaSourceXPath, String mediaSourceDataElementName, String contactJID) throws Exception {
		List<?> mediaSourceList = MRXMLUtils.getListXpath(roomdoc, mediaSourceXPath);
		List<Element> sourceList = new ArrayList<Element>();
		for(Iterator<?> i = mediaSourceList.iterator(); i.hasNext();) {
			Element mediaSourceDataElement = (Element)i.next();
			mediaSourceDataElement.element(mediaSourceDataElementName).setText(contactJID);
		 	MRXMLUtils.addAttribute(mediaSourceDataElement.getParent(),MediaRoomServiceConst.PRESENCE, MediaRoomServiceConst.AVAILABLE);
		 	sourceList.add(mediaSourceDataElement.getParent());
		}
		return sourceList;
		// System.out.println(MRXMLUtils.documentToString(roomdoc));
    }

    public synchronized List<Element> updateMediaDestContactJID(String mediaDestXPath, String mediaDestDataElementName, String contactJID) throws Exception {
		List<?> mediaDestList = MRXMLUtils.getListXpath(roomdoc, mediaDestXPath);
		List<Element> dstList = new ArrayList<Element>();
		
		for(Iterator<?> i = mediaDestList.iterator(); i.hasNext();) {
			Element mediaDestDataElement = (Element)i.next();
			// System.out.println("RoomDB:updateContactJID stream=" + MRXMLUtils.elementToString(stream));
			mediaDestDataElement.element(mediaDestDataElementName).setText(contactJID);
			MRXMLUtils.addAttribute(mediaDestDataElement.getParent(),MediaRoomServiceConst.PRESENCE, MediaRoomServiceConst.AVAILABLE);
			dstList.add(mediaDestDataElement.getParent());
		}
		return dstList;
		// System.out.println(MRXMLUtils.documentToString(roomdoc));
    }

    public synchronized void updateMediaRelayContactJID(String mediaRelayXPath, String mediaRelayDataElementName, String contactJID) throws Exception {
		List<?> mediaDestList = MRXMLUtils.getListXpath(roomdoc, mediaRelayXPath);
		for(Iterator<?> i = mediaDestList.iterator(); i.hasNext();) {
			Element mediaRelayDataElement = (Element)i.next();
			 mediaRelayDataElement.element(mediaRelayDataElementName).setText(contactJID);
			 MRXMLUtils.addAttribute(mediaRelayDataElement.getParent(),MediaRoomServiceConst.PRESENCE, MediaRoomServiceConst.AVAILABLE);
		}
	}
    	
    public synchronized Element deleteElement(MRRequestI request, MRLog log, MRDB db, String NID)
    throws Exception {
		Element element = this.readElementInternal(request, log, db, NID);
		Element parent = MRXMLUtils.getParentElement(element);
		parent.remove(element);
		return element;
    }

    public synchronized Element updateStreamStatus(MRRequestI request, MRLog log, MRDB db, String streamNID, 
    		Element streamStatus) throws Exception {
		XMPPTransaction.startBlock("readElementInternal");
    	// Element element = readElementInternal(request, log, db, streamNID);
		String xpath = "//Stream[@NID='" + streamNID + "']";
    	Element element = this.readElementInternalByXpath(request, log, db, xpath);
    	
    	int state = Utils.getIntValue(MRXMLUtils.getValueXpath(element, ".//state", ""), MediaRoomServiceConst.STREAM_STOPPED);
    	//If state is REVOKED_XAUTH, drops all requests
    	if(state == MediaRoomServiceConst.REVOKED_XAUTH)
    		return null;
    	
    	Element statusElement = MRXMLUtils.getElement(element, "Status");
    	long currentEventClock = Utils.getLongValue(MRXMLUtils.getAttributeValue(streamStatus, "eventWallcolck",""),0);
    	long lastEventClock = Utils.getLongValue(MRXMLUtils.getAttributeValue(statusElement, "eventWallcolck",""),0);
    
    	//stale event drop the message
    	if(lastEventClock!=0 && currentEventClock  < lastEventClock) {   
    		log.addInfo("MediaRoom: UpdateStreamStatus Found stale notification for stream :="+streamNID + " XML:"+ streamStatus.asXML(), MRLog.INFO, MRLog.NOTIFY);
    		return null; 
    	}
		XMPPTransaction.endBlock("readElementInternal");
		XMPPTransaction.startBlock("removeChild");
    	MRXMLUtils.removeChild(element,"Status");
		XMPPTransaction.endBlock("removeChild");
		XMPPTransaction.startBlock("clone");
    	Element status = ((Element)streamStatus.clone());
		XMPPTransaction.endBlock("clone");
    	status.setName("Status");
		XMPPTransaction.startBlock("addElement");
    	MRXMLUtils.addElement(element,status);
		XMPPTransaction.endBlock("addElement");
    	return (Element)element.clone();
    }

    public synchronized Element updateStreamState(MRRequestI request, MRLog log, MRDB db, String streamNID, 
    		String state) throws Exception {
    	Element element = readElementInternal(request, log, db, streamNID);
    	MRXMLUtils.getElementXpath(element, ".//state").setText(state);
    	return (Element)element.clone();
    }

    public synchronized Element updatePresence(MRRequestI request, MRLog log, MRDB db, String NID, String value)
    throws Exception {
    	Element element = readElementInternal(request, log, db, NID);
    	MRXMLUtils.addAttribute(element,MediaRoomServiceConst.PRESENCE, value);
		return (Element)element.clone();
    }



}
