package com.barco.device.ext.provider;

import java.util.Properties;

import org.xmlpull.v1.XmlPullParser;

public class BarcoXmppParser  {
	
	public static String BARCONS = "com.barco.agentmessage";

	public String parse(XmlPullParser parser) throws Exception {
		
		if (parser.getEventType() == XmlPullParser.TEXT)               
			return parser.getText();
			
		StringBuffer xml = new StringBuffer();
		String rootName = parser.getName();
		boolean emptyTag = false;
		while(true) {
			// TBD test and handle single tag in IQ
			// TBD test and handle xml control chars in IQ
			emptyTag = this.consumeToken(parser, xml, emptyTag);
			if(		XmlPullParser.END_TAG == parser.getEventType() && 
					rootName.equals(parser.getName()))
				break;
				parser.next();
		}
		return xml.toString();
	}
	
	private boolean consumeToken(XmlPullParser parser, StringBuffer xml, boolean emptyTag) throws Exception{
		if (parser.getEventType() == XmlPullParser.START_TAG) {
			xml.append("<").append(parser.getName());  
			int n = parser.getAttributeCount();    
			for (int i = 0; i < n; i++) {
				xml.append(" ").append(         
						parser.getAttributeName(i)).append("=\"") 
						.append(escapeXMLText(parser.getAttributeValue(i))) 
						.append("\"");               
			}                                 
			if (parser.isEmptyElementTag()) {    
				xml.append("/>");        
				emptyTag = true;              
			} else                           
				xml.append(">");     
		} else if (parser.getEventType() == XmlPullParser.END_TAG) {
			if (emptyTag)            
				emptyTag = false;     
			else                      
				xml.append("</").append(parser.getName()).append(">");             
		} else if (parser.getEventType() == XmlPullParser.TEXT) {                 
				xml.append(escapeXMLText(parser.getText()));                             
		}  
		return emptyTag;
	}
	
	private String escapeXMLText(String textValue){
		textValue = textValue.replaceAll("&", "&amp;");
		return textValue.replaceAll("<", "&lt;");
	}

	public static Properties parseProperties(XmlPullParser parser) throws Exception {
		parser.nextToken();
		int n = parser.getAttributeCount();
		Properties p = new Properties();
		for (int i = 0; i < n; i++) {     
			p.setProperty(parser.getAttributeName(i), parser.getAttributeValue(i)); 
		}
		return p;
	}

}
