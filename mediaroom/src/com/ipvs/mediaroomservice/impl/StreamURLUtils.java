package com.ipvs.mediaroomservice.impl;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.streamingserver.UDPSSAPI;

public final class StreamURLUtils {

	public static final boolean isValidIP(String ip) {
		Matcher m = Pattern.compile("(\\d+).(\\d+).(\\d+).(\\d+)").matcher(ip);
		return m.matches();
	}
	public static final void validateRTPStreamURL(String streamURL, MRLog log) throws Exception{
		  Document sdpDoc = SDPUtils.parseSDP_JainParser(streamURL, log);
		  String videoDestIP = MRXMLUtils.getValueXpath(sdpDoc, "//videoDestIP", "");
		  String audioDestIP = MRXMLUtils.getValueXpath(sdpDoc, "//audioDestIP", "");
		  if(!isValidIP(videoDestIP) && !isValidIP(audioDestIP))
			  throw new MRException(MediaRoomServiceExceptionCodes.INVALID_DESTIP,
					  "<destIP>" + videoDestIP + "</destIP>");
	}
	
	public static final void validateUDPStreamURL(String streamURL, MRLog log) throws Exception{
		  Document udpDoc = UDPSSAPI.parseUDPURL(streamURL, log);
		  String protocol = MRXMLUtils.getValueXpath(udpDoc, "//protocol", "");
		  if(!"udp".equals(protocol) && !"igmp".equals(protocol))
			  throw new MRException(MediaRoomServiceExceptionCodes.INVALID_PROTOCOL,
					  "<protocol>" + protocol + "</protocol>");
		  String destIP = MRXMLUtils.getValueXpath(udpDoc, "//destIP", "");
		  if(!isValidIP(destIP))
			  throw new MRException(MediaRoomServiceExceptionCodes.INVALID_DESTIP,
					  "<destIP>" + destIP + "</destIP>");
		  String destPort = MRXMLUtils.getValueXpath(udpDoc, "//destPort", "");
		  try {
			  Integer.parseInt(destPort);
		  }
		  catch(Exception e) {
			  throw new MRException(MediaRoomServiceExceptionCodes.INVALID_DESTPORT,
					  "<destPort>" + destPort + "</destPort>");
		  }
	}

}
