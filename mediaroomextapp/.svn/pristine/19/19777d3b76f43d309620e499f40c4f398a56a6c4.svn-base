package com.barco.utils.mr;

import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

public final class SDPUtils {

    public static final String SDP_XML = "/com/ipvs/mediaroomservice/impl/SDP.xml";
    public static final String SDP_STATIC_MAP = "/com/ipvs/mediaroomservice/impl/SDPStaticMap.xml";
    public static final String SAMPLESDP0 = "/com/ipvs/mediaroomservice/impl/sampleSDP.txt";
    public static final String SAMPLESDP1 = "/com/ipvs/mediaroomservice/impl/muaudio.sdp";
    public static final String SAMPLESDP2 = "/com/ipvs/mediaroomservice/impl/gaudio.sdp";
    public static final String SAMPLESDP3 = "/com/ipvs/mediaroomservice/impl/noaudio.sdp";
    public static final String SAMPLESDP4 = "/com/ipvs/mediaroomservice/impl/both.sdp";
    public static final String SAMPLESDP5 = "/com/ipvs/mediaroomservice/impl/unicast.sdp";
    public static final String SAMPLESDP6 = "/com/ipvs/mediaroomservice/impl/sampleTranscoderSDP.txt";

    public static final String DESTIP_TOKEN = "DESTIP_TOKEN";
    public static final String VIDEOPORT_TOKEN = "VIDEOPORT_TOKEN";
    public static final String AUDIOPORT_TOKEN = "AUDIOPORT_TOKEN";
    
    public static Document sdpStaticMap = null;
    
    public static Element getMapElement(String map, MRLog log) throws Exception {
    	if(SDPUtils.sdpStaticMap == null) {
    		SDPUtils.sdpStaticMap = MRXMLUtils.loadXML(SDP_STATIC_MAP, log);
    	}
    	return MRXMLUtils.getElementXpath(SDPUtils.sdpStaticMap, 
    			"//MapItem[@payloadType='" + map + "']");
    }

    public static String getSDPTemplateFromSDP(String sdp, MRLog log) throws Exception {
    	return SDPUtils.tokenizeSDP(sdp, log);
    }
    
    public static String generateSDPFromSDPTemplate(String sdptemplate, String destIP, int destUDPStartPort) 
    	throws Exception {
    	int videoPort = destUDPStartPort;
    	int audioPort = videoPort + 2;
    	String sdp = sdptemplate;
    	// Replace the values
    	sdp = sdp.replace(SDPUtils.DESTIP_TOKEN, destIP);
    	sdp = sdp.replace(SDPUtils.VIDEOPORT_TOKEN, String.valueOf(videoPort));
    	sdp = sdp.replace(SDPUtils.AUDIOPORT_TOKEN, String.valueOf(audioPort));
    	return sdp;
    }
        
    public static String getSDPFromStreamURL(String streamURL) {
    	return streamURL;
    }
    
    // SDP Parsing methods
    private static void processMediaStream(String sdp, Document doc, 
    		String regex, String mediaPortTag, String mediaMapTag) throws Exception {
    	
		Matcher m = Pattern.compile(regex).matcher(sdp);
		if(!m.find()) {
			return;
		}
		
		String portValue = m.group(2);
		try {
			// The portValue may have extra characters
			// System.out.println("SDPUtils:processMediaStream portValue=" + portValue);
			Matcher m1 = Pattern.compile("(\\d+)/(\\d+)").matcher(portValue);
			if(m1.find()) {
				int portNumber = Integer.parseInt(m1.group(1));
				portValue = String.valueOf(portNumber);
				// System.out.println("SDPUtils:processMediaStream portNumber=" + portNumber);
			}
		}
		catch(Exception e) {
			// System.out.println("SDPUtils:processMediaStream portValue error=" + e.toString());
		}
		
		
		MRXMLUtils.setValueXpath(doc, mediaPortTag, portValue);
		String mapValue = m.group(3);
		MRXMLUtils.setValueXpath(doc, mediaMapTag, mapValue);		
    }


    public static Document parseSDP(String sdp, MRLog log) throws Exception {
    	try {
	        Document doc = MRXMLUtils.loadXMLResource(SDPUtils.SDP_XML, log);
	        // Document doc = MRXMLUtils.getDocument(MRDB.class.getResource(SDPUtils.SDP_XML));
	    	SDPUtils.processMediaStream(sdp, doc, 
	    			"(m=audio)\\s+(\\d+)\\s+RTP/AVP\\s+(\\S+)", "//audioPort", "//audioMap");
	    			
	    	SDPUtils.processMediaStream(sdp, doc, 
	    	//		"(m=video)\\s+(\\d+)\\s+RTP/AVP\\s+(\\S+)","//videoPort", "//videoMap");
	    			"(m=video)\\s+([^\\s]+)\\s+RTP/AVP\\s+(\\S+)","//videoPort", "//videoMap");
	    	String audioMap = MRXMLUtils.getValueXpath(doc, "//audioMap", "");

	    	/*
	    	SDPUtils.processMediaStream(sdp, doc, 
	    			"(a=rtpmap:" + audioMap + ")\\s+(\\S+)/(\\d+)/(\\d+)", "//audioEncoding", "//audioClockRate");
	    	String videoMap = MRXMLUtils.getValueXpath(doc, "//videoMap", "");
	    	SDPUtils.processMediaStream(sdp, doc, 
	    			"(a=rtpmap:" + videoMap + ")\\s+(\\S+)/(\\d+)", "//videoEncoding", "//videoClockRate");
			*/
	    	
	    	if(!"".equals(audioMap)) {
		    	SDPUtils.processMediaStream(sdp, doc, 
		    			// "(a=rtpmap:" + audioMap + ")\\s+(\\S+)/(\\d+)/(\\d+)", "//audioEncoding", "//audioClockRate");
		    			"(a=rtpmap:" + audioMap + ")\\s+([^/]+)/(\\d+)", "//audioEncoding", "//audioClockRate");
		    	Element mapItem = SDPUtils.getMapElement(audioMap, log);
		    	if(mapItem != null) {
		    		MRXMLUtils.setValueXpath(doc, "//audioEncoding", MRXMLUtils.getAttributeValue(mapItem,"encoding"));
		    		MRXMLUtils.setValueXpath(doc, "//audioClockRate", MRXMLUtils.getAttributeValue(mapItem,"clockrate"));
		    	}
	    	}
	    	String videoMap = MRXMLUtils.getValueXpath(doc, "//videoMap", "");
	    	if(!"".equals(videoMap)) {
		    	SDPUtils.processMediaStream(sdp, doc, 
		    			// "(a=rtpmap:" + videoMap + ")\\s+(\\S+)/(\\d+)", "//videoEncoding", "//videoClockRate");
						"(a=rtpmap:" + videoMap + ")\\s+([^/]+)/(\\d+)", "//videoEncoding", "//videoClockRate");
		    	Element mapItem = SDPUtils.getMapElement(videoMap, log);
		    	if(mapItem != null) {
		    		MRXMLUtils.setValueXpath(doc, "//videoEncoding", MRXMLUtils.getAttributeValue(mapItem,"encoding"));
		    		MRXMLUtils.setValueXpath(doc, "//videoClockRate", MRXMLUtils.getAttributeValue(mapItem,"clockrate"));
		    	}
	    	}
			Matcher m = Pattern.compile("(c=IN\\s+IP4)\\s+([0-9\\.]+)").matcher(sdp);
			m.find();
			String destIPValue = m.group(2);
			MRXMLUtils.setValueXpath(doc, "//destIP", destIPValue);
			// System.out.println("SDPUtils:parseSDP doc=" + MRXMLUtils.documentToString(doc));
	    	return doc;
    	}
    	catch(Exception e) {
    		e.printStackTrace();
    		System.out.println("SDPUtils:parseSDP invalid SDP=" + sdp);
    		throw new MRException(MediaRoomServiceExceptionCodes.INVALID_SDP, "<sdp>" + sdp + "</sdp>");
    	}
    }

    private static String tokenize(String sdp, String regex, String replacement) throws Exception {    	
		Matcher m = Pattern.compile(regex).matcher(sdp);
		if(!m.find())
			return sdp;
    	StringBuffer sb = new StringBuffer();
		m.appendReplacement(sb, replacement);
    	while(m.find()) {
    		m.appendReplacement(sb, replacement);
    	}
    	m.appendTail(sb);
		return sb.toString();
    }

    public static String tokenizeSDP(String sdp, MRLog log) throws Exception {
		sdp = SDPUtils.tokenize(sdp, "(c=IN\\s+IP4)\\s+([0-9\\.]+)", "c=IN IP4 " + SDPUtils.DESTIP_TOKEN);
		
		sdp = SDPUtils.tokenize(sdp, "(m=audio)\\s+([^\\s]+)\\s+", "m=audio " + SDPUtils.AUDIOPORT_TOKEN + " ");
		sdp = SDPUtils.tokenize(sdp, "(m=video)\\s+([^\\s]+)\\s+", "m=video " + SDPUtils.VIDEOPORT_TOKEN + " ");

		// sdp = SDPUtils.tokenize(sdp, "(m=audio)\\s+(\\d+)\\s+", "m=audio " + SDPUtils.AUDIOPORT_TOKEN + " ");
		// sdp = SDPUtils.tokenize(sdp, "(m=video)\\s+(\\d+)\\s+", "m=video " + SDPUtils.VIDEOPORT_TOKEN + " ");
    	return sdp;
    }

    public static String getSampleSDP(String sampleFileName) throws Exception {
        URL url = SDPUtils.class.getResource(sampleFileName);
        InputStream in = url.openStream();
        byte b[] = new byte[in.available()];
        DataInputStream din = new DataInputStream(in);
        din.readFully(b);
        String sdp = new String(b);
        return sdp;
    }
    
    public static void testSDP(String sdpFileName) throws Exception {
    	String sdp = SDPUtils.getSampleSDP(sdpFileName);
    	System.out.println(sdp);
    	Document doc = SDPUtils.parseSDP(sdp, null);
    	System.out.println(MRXMLUtils.documentToString(doc));
    	String s = SDPUtils.tokenizeSDP(sdp, null);
    	System.out.println(s);
    }

    public static void main(String[] args) throws Exception {
    	/*
    	String sdp = "<streamURL><Locater/><Content>" + SDPUtils.getSampleSDP() + "</Content></streamURL>";
    	Document doc = MRXMLUtils.stringToDocument(sdp);
    	System.out.println(MRXMLUtils.documentToString(doc));
    	*/
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP0);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP1);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP2);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP3);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP4);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP5);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP6);
    }

	public static String fixsdp(String s) {
		
		// Insert a new line before "a="
		Matcher m = Pattern.compile("(\\s+)(\\S=)").matcher(s);
		StringBuffer sb = new StringBuffer();
		while(m.find())
			m.appendReplacement(sb, "\n" + m.group(2));
		m.appendTail(sb);
		
		// Insert a new line before the new line
		s = sb.toString();
		m = Pattern.compile("(\n)").matcher(s);
		sb = new StringBuffer();
		while(m.find())
			m.appendReplacement(sb, "\n" + m.group(1));
		m.appendTail(sb);
		
		return sb.toString();
	}
	
	
    
    public static String fixSDPProblemWithOlderMedia(String sdp) throws MRException,Exception{
     	//Added for older MPEG4 streams
		sdp = SDPUtils.tokenize(sdp, "(c=IN\\s+IP4)\\s+(SERVERIP)", "c=IN IP4 " + SDPUtils.DESTIP_TOKEN);   
		sdp = SDPUtils.tokenize(sdp, "(m=audio)\\s+(AUDIO_PORT)\\s+", "m=audio " + SDPUtils.AUDIOPORT_TOKEN + " ");
		sdp = SDPUtils.tokenize(sdp, "(m=video)\\s+(VIDEO_PORT)\\s+", "m=video " + SDPUtils.VIDEOPORT_TOKEN + " ");
		
	    //remove extra tab space
		if(sdp.endsWith("\t")){
			sdp = sdp.substring(0,sdp.lastIndexOf("\t"));
		}
	 
		sdp = fixsdp(sdp);
		
	    return sdp;
    }

}
