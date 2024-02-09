package com.barco.device.vng.Media.streamingserver;

import org.apache.log4j.Logger;
import org.dom4j.Document;

import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;


public class MPEGTSSSAPI extends UDPSSAPI {
		
	protected String getInputPortType(String inputType) throws ServiceException {
        if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            return MediaRoomServiceConst.DMS_MEDIA_PORT; // TBD Was also returning V2DMediaPort
        } else if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
            return MediaRoomServiceConst.MPEGTS_NETWORK_PORT;
        } else if (MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            return MediaRoomServiceConst.MPEGTS_NETWORK_PORT;
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
		return "multicastip=" + destIP + " dstport=" + destPort;
	}

	protected String getOutputPortType(String outputType) {
	    if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
	    		MediaRoomServiceConst.OUTPUT_PVR.equals(outputType)) {
	    	return MediaRoomServiceConst.DMS_RECORDER_PORT;	
	    } else {
	        return MediaRoomServiceConst.MPEGTS_DECODER_PORT;
	    }
	}

	protected String getStreamType() {
		return MediaRoomServiceConst.MPEGTS;
	}

}
