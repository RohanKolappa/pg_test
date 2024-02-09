package com.barco.device.handler.Media.impl.streamingserver;

import org.dom4j.Document;

import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRXMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;
import com.barco.utils.mr.SDPUtils;

public class RTPSSAPI extends SSAPI {

	protected String getInputPortType(String inputType) throws MRException {
        if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            return MediaRoomServiceConst.DMS_MEDIA_PORT; // TBD Was also returning V2DMediaPort
        } else if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
            return MediaRoomServiceConst.RTP_NETWORK_PORT;
        } else if (MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
            return MediaRoomServiceConst.RTP_NETWORK_PORT;
        } else {
            throw new MRException(MediaRoomServiceExceptionCodes.INPUTTYPENOTFOUND, MRXMLUtils.generateErrorXML("inputType", inputType));
        }		
	}

	protected String getNetworkInputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String streamURL, MRLog log) throws Exception {
		
        String streamURLType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamURLType", "");
        if(MediaRoomServiceConst.HTTP.equals(streamURLType))
        	return streamURL;
		String sdp = SDPUtils.getSDPFromStreamURL(streamURL);
		Document sdpDoc = SDPUtils.parseSDP(sdp, log);
		String sdpDestIP = MRXMLUtils.getValueXpath(sdpDoc, "//destIP", "");
		String audioPort = MRXMLUtils.getValueXpath(sdpDoc, "//audioPort", "");
		String videoPort = MRXMLUtils.getValueXpath(sdpDoc, "//videoPort", "");
		
		String videoEncoding = MRXMLUtils.getValueXpath(sdpDoc, "//videoEncoding", "");
		String audioEncoding = MRXMLUtils.getValueXpath(sdpDoc, "//audioEncoding", "");
		
		StringBuffer sb = new StringBuffer("srcip=" + sdpDestIP);
		int videoPortNumber = 0;
		try { videoPortNumber = Integer.parseInt(videoPort); } catch(Exception e){}
		int audioPortNumber = 0;
		try { audioPortNumber = Integer.parseInt(audioPort); } catch(Exception e){}
		if(videoPortNumber > 0)
			sb.append(" videoport=" + videoPort + " videocodec=" + videoEncoding);
		if(audioPortNumber > 0)
			sb.append(" audioport=" + audioPort + " audiocodec=" + audioEncoding);
		sb.append(" ");
		return sb.toString();
	}

	protected String getNetworkOutputString(String ssInstanceID,
			Document streamDataDoc, NetworkResource networkResource,
			String IPAddress, String inputType, String inputInstanceID, MRLog log,
			StringBuffer streamURLReturn) throws Exception {
		String destIP = this.getResolvedDestIPAddress(streamDataDoc);
		if(destIP == null || "".equalsIgnoreCase(destIP)) {
			destIP = MRXMLUtils.getValueXpath(streamDataDoc, "//destIP", "");
		}	
		//System.out.println(destIP);
		String destUDPStartPort = MRXMLUtils.getValueXpath(streamDataDoc, "//destUDPStartPort", "");
		int destUDPStartPortNumber = Integer.parseInt(destUDPStartPort);
		String sdptemplate = MRXMLUtils.getValueXpath(streamDataDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE, "");
		String sdp = SDPUtils.generateSDPFromSDPTemplate(sdptemplate, destIP, destUDPStartPortNumber);
		streamURLReturn.append(sdp);
		Document sdpDoc = SDPUtils.parseSDP(sdp, log);
		String videoClockRate = MRXMLUtils.getValueXpath(sdpDoc, "//videoClockRate", "");
		String audioClockRate = MRXMLUtils.getValueXpath(sdpDoc, "//audioClockRate", "");
		String mcastTTL = this.getMcastTTL();

		StringBuffer sb = new StringBuffer("dstaddr=" + destIP + " dstport=" + destUDPStartPort +
		" mcastttl=" + mcastTTL);
		if(!"".equals(videoClockRate))
			sb.append(" videoclockrate=" + videoClockRate);
		if(!"".equals(audioClockRate))
			sb.append(" audioclockrate=" + audioClockRate);
		sb.append(" ");
		return sb.toString();
	}

	protected String getOutputPortType(String outputType) {
	    if (MediaRoomServiceConst.OUTPUT_RECORD.equals(outputType) || 
	    		MediaRoomServiceConst.OUTPUT_PVR.equals(outputType)) {
	    	return MediaRoomServiceConst.DMS_RECORDER_PORT;	
	    } else {
	        return MediaRoomServiceConst.RTP_DECODER_PORT;
	    }
	}

	protected String getStreamType() {
		return MediaRoomServiceConst.RTP;
	}

	protected void releaseNetworkInputResources(
			NetworkResource networkResource, String ssInstanceID, MRLog log)
			throws Exception {
		// No resources allocated
	}

	protected void releaseNetworkOutputResources(
			NetworkResource networkResource, String ssInstanceID,
			Document streamDataDoc, MRLog log) throws Exception {
		// No resources allocated
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
		if (ipaddress == null || "".equalsIgnoreCase(ipaddress)) {
			return false;
			
		}
		return true;
	}

}
