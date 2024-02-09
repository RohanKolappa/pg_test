package com.barco.device.vng.Media.streamingserver;

import org.apache.log4j.Logger;
import org.dom4j.Document;

import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class DVISSAPI extends SSAPI {

	protected String getInputPortType(String inputType) throws ServiceException {
		if (MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)) {
            return MediaRoomServiceConst.DVI_ENCODER_PORT;
        } else if (MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            return MediaRoomServiceConst.DVI_TRANSPORT_PORT;
        } else {
            throw new ServiceException(MediaRoomServiceExceptionCodes.INPUTTYPENOTFOUND, XMLUtils.generateErrorXML("inputType", inputType));
        }		
	}

	protected String getNetworkInputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String streamURL, Logger log) throws Exception {
		return "dvi_input";
	}

	protected String getNetworkOutputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String IPAddress, String inputType, String inputInstanceID, Logger log,
			StringBuffer streamURLReturn) throws Exception {
		
		// TBD: Temp fix to get player working
        String bandwidth = "0";
        String tcpPort = "0";
        String isMulticast = "false";
        String enablekbm = "false";
        String avoption = "Video";
        String mediatype = "v2d";
        
        streamURLReturn.append(mediatype).append("://");
        streamURLReturn.append(IPAddress).append("/");
        streamURLReturn.append("port=").append(tcpPort);
        streamURLReturn.append("?bandwidth=").append(bandwidth);
        streamURLReturn.append("?avoption=").append(avoption);
        streamURLReturn.append("?ismulticast=").append(isMulticast);
        streamURLReturn.append("?enablekbm=").append(enablekbm);

		return "dvi_output";
	}

	protected String getOutputPortType(String outputType) throws Exception {
	    if(MediaRoomServiceConst.OUTPUT_DECODER.equalsIgnoreCase(outputType)) {
	    	return MediaRoomServiceConst.DVI_HARDWARE_DECODER_PORT;
        } else {
            return MediaRoomServiceConst.DVI_DECODER_PORT;
        }		
	}

	protected String getStreamType() {
		return MediaRoomServiceConst.DVI;
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

	@Override
	public boolean isValidDestIP(String ipaddress) {
		return true;
	}

	@Override
	public boolean isValidSourceIP(String ipaddress) {
		return true;
	}

}
