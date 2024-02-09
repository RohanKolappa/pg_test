package com.ipvs.test;

import org.dom4j.Document;
import org.dom4j.XPath;

import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class TestNS {
	
	public static void main(String[] args) throws Exception {
		String xml = "<query xmlns='com.ipvs.session'>" +
		    			"<Join sessionId='collaborationID' />" +
		    		 "</query>";
		Document doc = MRXMLUtils.stringToDocument(xml);
		//XPath xpath = XPath.newInstance("//ipvs:Join");
		//xpath.addNamespace("ipvs", IPVSMessageExtension.IPVS_SESSION_XMLNS);
	    XPath xpath  = MRXMLUtils.createXPath("//ipvs:Join", "ipvs", IPVSMessageExtension.IPVS_SESSION_XMLNS);
        		
		System.out.println(xpath.selectSingleNode(doc));
		/*
		XPath xpath = XPath.newInstance("//Join");
		Element data = (Element) xpath.selectSingleNode(doc);
		String sessionId = data.getAttributeValue("sessionId");
		System.out.println(sessionId);
		*/
	}

}
