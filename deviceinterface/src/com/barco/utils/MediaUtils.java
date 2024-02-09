package com.barco.utils;

import java.net.URLDecoder;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

public class MediaUtils {
	
	static String MEDIAROOM_FILE_PATH = "/com/ipvs/xsd/objectXml/MediaRoom.xml";
	static Document streamStatusTemplate = null;
	public static final String DESTIP_TOKEN = "DESTIP_TOKEN";
	public static final String VIDEOPORT_TOKEN = "VIDEOPORT_TOKEN";
	public static final String AUDIOPORT_TOKEN = "AUDIOPORT_TOKEN";

	public static Document createStreamStatusDoc() throws Exception  {
		if(streamStatusTemplate != null)
			return (Document)streamStatusTemplate.clone();
		Document mediaRoomDoc = XMLUtils.loadXMLResource(MEDIAROOM_FILE_PATH);
		Element streamElement = XMLUtils.getElementXpath(mediaRoomDoc, "//Stream");
		Element streamStatusElement = streamElement.element("Status");
		Element urlElement = streamStatusElement.element("URL");
		if(urlElement == null)
			streamStatusElement.add(XMLUtils.stringToElement("<URL/>"));
		streamStatusTemplate = XMLUtils.elementToDocument(streamStatusElement);
		streamStatusTemplate.getRootElement().setName("StreamStatus");
		return (Document)streamStatusTemplate.clone();
	}
	
	public static Element createMediaTimelineElement() throws Exception {
		Document streamStatusDoc = MediaUtils.createStreamStatusDoc();
		return (Element)XMLUtils.getElementXpath(streamStatusDoc, "//MediaTimeline").detach();
	}

	public static Document parseDataDoc(Element element) throws Exception {
		Element dataElement = element.element("data");
		if(dataElement != null)
			return XMLUtils.elementToDocument(dataElement);
		String s = URLDecoder.decode(element.getText(), "UTF-8");
        return DocumentHelper.parseText(s);
	}
	
	private static String tokenize(String sdp, String regex, String replacement)
			throws Exception {
		Matcher m = Pattern.compile(regex).matcher(sdp);
		if (!m.find())
			return sdp;
		StringBuffer sb = new StringBuffer();
		m.appendReplacement(sb, replacement);
		while (m.find()) {
			m.appendReplacement(sb, replacement);
		}
		m.appendTail(sb);
		return sb.toString();
	}

	public static String tokenizeSDP(String sdp, Logger log) throws Exception {
		sdp = MediaUtils.tokenize(sdp, "(c=IN\\s+IP4)\\s+([0-9\\.]+)",
				"c=IN IP4 " + MediaUtils.DESTIP_TOKEN);
		sdp = MediaUtils.tokenize(sdp, "(m=audio)\\s+([^\\s]+)\\s+", "m=audio "
				+ MediaUtils.AUDIOPORT_TOKEN + " ");
		sdp = MediaUtils.tokenize(sdp, "(m=video)\\s+([^\\s]+)\\s+", "m=video "
				+ MediaUtils.VIDEOPORT_TOKEN + " ");
		return sdp;
	}

}
