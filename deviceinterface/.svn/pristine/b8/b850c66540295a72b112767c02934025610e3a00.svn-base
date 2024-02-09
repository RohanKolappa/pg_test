package com.barco.device.handler.Layout;



import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.common.StreamEntry;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class TeardownRequestHandler implements MessageHandlerI, ResponseHandlerI{

	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");
        
        StreamEntry se = this.teardownStream(device, streamNID);
        
        Document statusDoc = XMLUtils.elementToDocument(se.status);
        message.setResult(statusDoc);

	}

	public StreamEntry teardownStream(DeviceI device, String streamNID) throws Exception {
	    StreamEntry se = device.getDeviceDB().getStreamCache().deleteStreamEntry(streamNID);
		if(se == null)
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTFOUND, "TeardownRequestHandler:handleMessage streamNID=" + streamNID);
	
		String streamURL = XMLUtils.getValueXpath(se.streamDataDoc, "//streamURL", "");
	    if(!"".equals(streamURL)) { // This was signaled
	    	this.stopPlaying(device, streamNID);
	    	return se;
	    }
	    
	    // Send StopRequest
		String requestNID = "";
		String clientdata = "";
		String clientcallback = "";
		this.stopPlaying(device, streamNID);
		String data =  "<StopRequestData>" + 
                "<streamNID>" + streamNID + "</streamNID>" +
                "</StopRequestData>";
		
		try {
			Document requestDocument = XMLUtils.createRequestDocument("Media", "StopRequest", device.getMyJID(),
					requestNID, clientdata, clientcallback, data);
			Message startRequestMessage = Message.createRequest(requestDocument, se.sourceAgentJID, device.getMyJID());
			device.sendRequest(startRequestMessage, requestDocument, this);
		}
		catch(Exception e) {
			device.getLogger().error("TeardownStreamRequestHandler:deleteStreamEntry streamNID=" + streamNID);
		}
		
		String destType = XMLUtils.getValueXpath(se.streamDataDoc, "//destType", "");
		double timescale = Utils.getDoubleValue(XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpathFromElement(se.status, ".//Last"), "TimeScale", ""), 0);

		// check if we need to release destination UDP port
		this.releaseDestIPPort(device, se.streamDataDoc);
		
		//recording paused we have to simulate ss event
		if(destType.equals("RECORD") && timescale == 0){			
			Document eventDoc = DocumentHelper.createDocument();			
			Element eventElement = DocumentHelper.createElement("SSEvent");
			eventElement.addAttribute("fileUUID",  XMLUtils.getValueXpath(se.streamDataDoc, "//destFileUUID", ""));
			eventElement.addAttribute("dirPath",  XMLUtils.getValueXpath(se.streamDataDoc, "//destFileParentDirPath", ""));
			eventElement.addAttribute("dirNID",  XMLUtils.getValueXpath(se.streamDataDoc, "//destFileParentDirNID", ""));
			eventElement.addAttribute("playLength",  XMLUtils.getValueXpath(se.streamDataDoc, "//destFilePlaylength", ""));
			eventElement.addAttribute("size",  XMLUtils.getValueXpath(se.streamDataDoc, "//destFileSize", ""));
			eventDoc.setRootElement(eventElement);			
			new SSEventHandler().handleStreamStopped( device, eventElement);
		}
		
		return se;
	}

	@Override
	public void handleResponse(Message message, Document context, DeviceI device)
			throws Exception {
		// device.addLog("TeardownStreamRequestHandler:handleResponse xml=" + message.getMessageDoc().asXML(), LoggerI.DEBUG);
	}
	@Override
	public void handleError(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}
	
	public void stopPlaying(DeviceI device, String streamNID) {
		device.getLayoutAgentImpl().stopPlaying(streamNID);
		try {
			device.getMediaAgentImpl().stopPlaying("Layout." + streamNID);
		}
		catch(Exception e) {
			e.printStackTrace();
			device.getLogger().error("TeardownRequestHandler:stopPlaying error=" + e.toString());
		}
	}
	
	private boolean needsUDPDestPort(String streamType,String destType) {
    	return ((!("PVR").equals(destType) 
    			&& !("RECORD").equals(destType) ) 
    			&& (("RTP").equals(streamType) ||
    			("MPEGTS").equals(streamType) ||
    			("UDP").equals(streamType)));
    }
	
	private void releaseDestIPPort(DeviceI device, Document streamDataDoc) throws Exception { // TBD
        // Release the destIP and destPort
        String streamType = XMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
        String destType = XMLUtils.getValueXpath(streamDataDoc, "//destType", "");
        
        if(!this.needsUDPDestPort(streamType,destType))
        	return;
        
    	String destStreamURL = XMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
    	// TBD:: Should check for WebClient-Device?
    	if(!"".equals(destStreamURL)) // Is Un Managed destination 
    		return;

		String streamURL = XMLUtils.getValueXpath(streamDataDoc, "//streamURL",	"");
		if (!"".equals(streamURL))
			return;
	
		String destUDPStartPort = XMLUtils.getValueXpath(streamDataDoc, "//destUDPStartPort", "");
		device.getLogger().debug("TeardownRequestHandler:releaseDestIPPort Un-allocating port destUDPStartPort=" + destUDPStartPort + " destType=" + destType + " streamType=" + streamType);
		device.getMediaAgentImpl().releaseUDPStartPort(destUDPStartPort, streamType);
    }

	
}
