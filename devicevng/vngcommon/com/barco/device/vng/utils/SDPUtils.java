package com.barco.device.vng.utils;

import java.io.DataInputStream;
import java.io.InputStream;
import java.net.URL;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.sdp.Attribute;
import javax.sdp.MediaDescription;
import javax.sdp.SdpFactory;
import javax.sdp.SessionDescription;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public final class SDPUtils {

    public static final String SDP_STATIC_MAP = "/com/barco/device/vng/utils/SDPStaticMap.xml";
    public static final String SDP_XML = "/com/barco/device/vng/utils/SDP.xml";
    /*
    public static final String SAMPLESDP0 = "/com/barco/device/vng/Media/sampleSDP.txt";
    public static final String SAMPLESDP1 = "/com/barco/device/vng/Media/muaudio.sdp";
    public static final String SAMPLESDP2 = "/com/barco/device/vng/Media/gaudio.sdp";
    public static final String SAMPLESDP3 = "/com/barco/device/vng/Media/noaudio.sdp";
    public static final String SAMPLESDP4 = "/com/barco/device/vng/Media/both.sdp";
    public static final String SAMPLESDP5 = "/com/barco/device/vng/Media/unicast.sdp";
    public static final String SAMPLESDP6 = "/com/barco/device/vng/Media/sampleTranscoderSDP.txt";
    */

    public static final String DESTIP_TOKEN = "DESTIP_TOKEN";
    public static final String VIDEOPORT_TOKEN = "VIDEOPORT_TOKEN";
    public static final String AUDIOPORT_TOKEN = "AUDIOPORT_TOKEN";
    
    public static Document sdpStaticMap = null;
    
    public static Element getMapElement(String map, Logger log) throws Exception {
    	if(SDPUtils.sdpStaticMap == null) {
    		SDPUtils.sdpStaticMap = XMLUtils.loadXMLResource(SDP_STATIC_MAP);
    	}
    	return XMLUtils.getElementXpath(SDPUtils.sdpStaticMap, 
    			"//MapItem[@payloadType='" + map + "']");
    }

    public static String getSDPTemplateFromSDP(String sdp, Logger log) throws Exception {
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
		
		
		XMLUtils.setValueXpath(doc, mediaPortTag, portValue);
		String mapValue = m.group(3);
		XMLUtils.setValueXpath(doc, mediaMapTag, mapValue);		
    }
  private static void updateMediaDescriptionParams(String mediaType,MediaDescription md, Document sdpDoc,String sessconnectionAddress) throws Exception {
    	
		
    	//update connection info. If media connection info not found use session connection in case session connection is not 0.0.0.0 or localhost
		if(md.getConnection()!=null) {
			XMLUtils.setValueXpath(sdpDoc, "//"+mediaType+"DestIP", md.getConnection().getAddress());
		} else if(sessconnectionAddress != null && !sessconnectionAddress.isEmpty() && !sessconnectionAddress.equals("0.0.0.0") && !sessconnectionAddress.equals("127.0.0.1")) {
			XMLUtils.setValueXpath(sdpDoc, "//"+mediaType+"DestIP", sessconnectionAddress);
		}
		
		//update port and map information
		XMLUtils.setValueXpath(sdpDoc, "//"+mediaType+"Port", String.valueOf(md.getMedia().getMediaPort()));
    	if(md.getMedia().getMediaFormats(false) !=null && md.getMedia().getMediaFormats(false).size() !=0)
    		XMLUtils.setValueXpath(sdpDoc, "//"+mediaType+"Map", String.valueOf(md.getMedia().getMediaFormats(false).get(0)));

    	String rtpmap =  md.getAttribute("rtpmap");

    	//update encoding and clock information
		if(rtpmap!=null) {
			String[] splitStr = rtpmap.split("/");
			if(splitStr.length != 0 ) {
				XMLUtils.setValueXpath(sdpDoc, "//"+mediaType+"Encoding", splitStr[0].substring(splitStr[0].indexOf("\\s+")+1));
				if(splitStr.length >=1) {
					XMLUtils.setValueXpath(sdpDoc, "//"+mediaType+"ClockRate", splitStr[1]);				
				}
			}
		}
    	
    }    
    
    
    public static Document parseSDP_JainParser(String sdp,Logger log) throws Exception {
    	try {
    	    Document doc = XMLUtils.loadXMLResource(SDPUtils.SDP_XML);
    	   
    	    //fix the sdp in case if we do not find new line in the sdp. Jain parser needed new line between params
    	    if(!sdp.matches("\\n")) {
    	    	sdp=fixsdp(sdp);
    	    }
    	    
			SdpFactory sdpFactory = SdpFactory.getInstance();
			SessionDescription sessiondescription = sdpFactory
					.createSessionDescription(sdp);


			@SuppressWarnings("unchecked")
			Vector<Attribute> attrs = sessiondescription.getAttributes(false);

			if (attrs != null) {
				Attribute attrib = (Attribute) attrs.get(0);
				System.out.println("attrs = " + attrib.getName());
			}
			
			
			String sessionConnectionAddress = null;
			if(sessiondescription.getConnection()!=null) {
				System.out.println("Session Connection:="+sessiondescription.getConnection().getAddress());
				sessionConnectionAddress = sessiondescription.getConnection().getAddress();
			}			

			for(Object object : sessiondescription.getMediaDescriptions(false)) {

				MediaDescription md = (MediaDescription)object;
				
				if(!md.getMedia().getMediaType().equalsIgnoreCase("video") && !md.getMedia().getMediaType().equalsIgnoreCase("audio")) {
					continue;
					
				} 
				
				updateMediaDescriptionParams(md.getMedia().getMediaType().toLowerCase(), md, doc, sessionConnectionAddress);
			}
			
			System.out.println("doc :="+doc.asXML());

			return doc;

		} catch(Exception exp) {
			exp.printStackTrace();
			throw new ServiceException(MediaRoomServiceExceptionCodes.INVALID_SDP, "<sdp>" + sdp + "</sdp>");
		}
    }

    public static Document parseSDP(String sdp, Logger log) throws Exception {
    	try {
	        Document doc = XMLUtils.loadXMLResource(SDPUtils.SDP_XML);
	        // Document doc = MRXMLUtils.getDocument(MRDB.class.getResource(SDPUtils.SDP_XML));
	    	SDPUtils.processMediaStream(sdp, doc, 
	    			"(m=audio)\\s+(\\d+)\\s+RTP/AVP\\s+(\\S+)", "//audioPort", "//audioMap");
	    			
	    	SDPUtils.processMediaStream(sdp, doc, 
	    	//		"(m=video)\\s+(\\d+)\\s+RTP/AVP\\s+(\\S+)","//videoPort", "//videoMap");
	    			"(m=video)\\s+([^\\s]+)\\s+RTP/AVP\\s+(\\S+)","//videoPort", "//videoMap");
	    	String audioMap = XMLUtils.getValueXpath(doc, "//audioMap", "");

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
		    		XMLUtils.setValueXpath(doc, "//audioEncoding", XMLUtils.getAttributeValue(mapItem,"encoding"));
		    		XMLUtils.setValueXpath(doc, "//audioClockRate", XMLUtils.getAttributeValue(mapItem,"clockrate"));
		    	}
	    	}
	    	String videoMap = XMLUtils.getValueXpath(doc, "//videoMap", "");
	    	if(!"".equals(videoMap)) {
		    	SDPUtils.processMediaStream(sdp, doc, 
		    			// "(a=rtpmap:" + videoMap + ")\\s+(\\S+)/(\\d+)", "//videoEncoding", "//videoClockRate");
						"(a=rtpmap:" + videoMap + ")\\s+([^/]+)/(\\d+)", "//videoEncoding", "//videoClockRate");
		    	Element mapItem = SDPUtils.getMapElement(videoMap, log);
		    	if(mapItem != null) {
		    		XMLUtils.setValueXpath(doc, "//videoEncoding", XMLUtils.getAttributeValue(mapItem,"encoding"));
		    		XMLUtils.setValueXpath(doc, "//videoClockRate", XMLUtils.getAttributeValue(mapItem,"clockrate"));
		    	}
	    	}
			Matcher m = Pattern.compile("(c=IN\\s+IP4)\\s+([0-9\\.]+)").matcher(sdp);
			m.find();
			String destIPValue = m.group(2);
			XMLUtils.setValueXpath(doc, "//destIP", destIPValue);
			// System.out.println("SDPUtils:parseSDP doc=" + MRXMLUtils.documentToString(doc));
	    	return doc;
    	}
    	catch(Exception e) {
    		e.printStackTrace();
    		System.out.println("SDPUtils:parseSDP invalid SDP=" + sdp);
    		throw new ServiceException(MediaRoomServiceExceptionCodes.INVALID_SDP, "<sdp>" + sdp + "</sdp>");
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

    public static String tokenizeSDP(String sdp, Logger log) throws Exception {
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
    	System.out.println(XMLUtils.documentToString(doc));
    	String s = SDPUtils.tokenizeSDP(sdp, null);
    	System.out.println(s);
    }

    public static void main(String[] args) throws Exception {
    	/*
    	String sdp = "<streamURL><Locater/><Content>" + SDPUtils.getSampleSDP() + "</Content></streamURL>";
    	Document doc = MRXMLUtils.stringToDocument(sdp);
    	System.out.println(MRXMLUtils.documentToString(doc));
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP0);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP1);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP2);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP3);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP4);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP5);
    	SDPUtils.testSDP(SDPUtils.SAMPLESDP6);
    	*/
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
	
	
    
    public static String fixSDPProblemWithOlderMedia(String sdp) throws ServiceException,Exception{
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
