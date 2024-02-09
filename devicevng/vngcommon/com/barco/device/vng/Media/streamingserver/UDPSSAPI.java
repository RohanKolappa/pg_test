package com.barco.device.vng.Media.streamingserver;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;
import org.dom4j.Document;

import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class UDPSSAPI extends SSAPI {
	
    public static final String UDP_XML = "/com/barco/device/vng/Media/UDP.xml";


	public static final Document parseUDPURL(String UDPURL, Logger log) throws Exception {
        Document doc = XMLUtils.loadXMLResource(UDPSSAPI.UDP_XML);

		String regexp = "(\\S+)://(\\S+):(\\d+)";
		Matcher m = Pattern.compile(regexp).matcher(UDPURL);
		m.find();
		
		XMLUtils.getElementXpath(doc, "//protocol").setText(m.group(1));
		XMLUtils.getElementXpath(doc, "//destIP").setText(m.group(2));
		XMLUtils.getElementXpath(doc, "//destPort").setText(m.group(3));
		
		return doc;
	}
	
	protected String getInputPortType(String inputType) throws ServiceException {
        if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            return MediaRoomServiceConst.DMS_MEDIA_PORT; // TBD Was also returning V2DMediaPort
        } else if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
            return MediaRoomServiceConst.UDP_NETWORK_PORT;
        } else if (MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            return MediaRoomServiceConst.UDP_NETWORK_PORT;
        } else {
            throw new ServiceException(MediaRoomServiceExceptionCodes.INPUTTYPENOTFOUND, XMLUtils.generateErrorXML("inputType", inputType));
        }		
	}

	protected String getNetworkInputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String streamURL, Logger log) throws Exception {
		Document UDPDoc = UDPSSAPI.parseUDPURL(streamURL, log);
		// String protocol = MRXMLUtils.getValueXpath(UDPDoc, "//protocol", "");
		String destIP = XMLUtils.getValueXpath(UDPDoc, "//destIP", "");
		String destPort = XMLUtils.getValueXpath(UDPDoc, "//destPort", "");
		// return "mcastaddr=" + destIP + " srcport=" + destPort; // If multicast
		return "mcastaddr=" + destIP + " srcport=" + destPort;
	}

	protected String getNetworkOutputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String IPAddress, String inputType, String inputInstanceID, Logger log,
			StringBuffer streamURLReturn) throws Exception {
		//String destIP = MRXMLUtils.getValueXpath(streamDataDoc, "//destIP", "");
		String destIP = this.getResolvedDestIPAddress(streamDataDoc);
		if(destIP == null || "".equalsIgnoreCase(destIP)) {
			destIP = XMLUtils.getValueXpath(streamDataDoc, "//destIP", "");
		}
		String destUDPStartPort = XMLUtils.getValueXpath(streamDataDoc, "//destUDPStartPort", "");
		streamURLReturn.append(MediaRoomServiceConst.PROTOCOL_UDP + "://" + destIP + ":" + destUDPStartPort);
		String mcastTTL = this.getMcastTTL();
		
		// return "destaddr=" + destIP + " destPort=" + destUDPStartPort + " mcastttl=" + mcastTTL;
		return "dstaddr=" + destIP + " dstport=" + destUDPStartPort + 
				" mcastttl=" + mcastTTL;
	}

	protected String getOutputPortType(String outputType) {
	    if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
	    		MediaRoomServiceConst.OUTPUT_PVR.equals(outputType)) {
	    	return MediaRoomServiceConst.DMS_RECORDER_PORT;	
	    } else {
	        return MediaRoomServiceConst.UDP_DECODER_PORT;
	    }
	}

	protected String getStreamType() {
		return MediaRoomServiceConst.UDP;
	}

	protected void releaseNetworkInputResources(
			NetworkResource networkResource, String ssInstanceID, Logger log)
			throws Exception {
		// No resources allocated
	}

	protected void releaseNetworkOutputResources(
			NetworkResource networkResource, String ssInstanceID,
			Document streamDataDoc, Logger log) throws Exception {
		// No resources allocated
	}
	
	public static void main(String args[]) throws Exception {
		Document doc = UDPSSAPI.parseUDPURL("igmp://224.1.1.1:2001", null);
		System.out.println(XMLUtils.documentToString(doc));
	}



	@Override
	public boolean isValidDestIP(String ipaddress) {
		if (ipaddress == null || "".equalsIgnoreCase(ipaddress)) {
			return false;
			
		}
		return true;
	}

	@Override
	public boolean isValidSourceIP(String ipaddress) {
		return true;
	}

}
