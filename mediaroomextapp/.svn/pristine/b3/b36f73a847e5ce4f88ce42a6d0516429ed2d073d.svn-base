package com.barco.device.handler.Media.impl.streamingserver;

import org.dom4j.Document;

import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRXMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;


public class MPEGTSSSAPI extends UDPSSAPI {
		
	protected String getInputPortType(String inputType) throws MRException {
        if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            return MediaRoomServiceConst.DMS_MEDIA_PORT; // TBD Was also returning V2DMediaPort
        } else if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
            return MediaRoomServiceConst.MPEGTS_NETWORK_PORT;
        } else if (MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            return MediaRoomServiceConst.MPEGTS_NETWORK_PORT;
        } else {
            throw new MRException(MediaRoomServiceExceptionCodes.INPUTTYPENOTFOUND, MRXMLUtils.generateErrorXML("inputType", inputType));
        }		
	}

	protected String getNetworkInputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String streamURL, MRLog log) throws Exception {
		Document UDPDoc = UDPSSAPI.parseUDPURL(streamURL, log);
		// String protocol = MRXMLUtils.getValueXpath(UDPDoc, "//protocol", "");
		String destIP = MRXMLUtils.getValueXpath(UDPDoc, "//destIP", "");
		String destPort = MRXMLUtils.getValueXpath(UDPDoc, "//destPort", "");
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
