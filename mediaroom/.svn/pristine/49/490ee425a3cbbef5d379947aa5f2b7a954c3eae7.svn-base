package com.ipvs.common.service;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;

import org.dom4j.Document;

public final class BundleUtils {
    private static String OBJECTXML_PLUGIN_NAME = "model";
    
    public static Document loadBundleXMLResource(String xmlFilePath) throws Exception {
		int index = xmlFilePath.indexOf("objectXml");
		Document doc = null;
		if(index!=-1){
			xmlFilePath = xmlFilePath.substring(index);
		}
		URL url = new URL("platform:/plugin/"+OBJECTXML_PLUGIN_NAME+"/" + xmlFilePath);
		InputStream inputStream = url.openConnection().getInputStream();
		BufferedReader in = new BufferedReader(new InputStreamReader(inputStream));
		String inputLine;
		StringBuilder sb = new StringBuilder();
		while ((inputLine = in.readLine()) != null) {
			sb.append(inputLine);
		}
		in.close();
		doc = MRXMLUtils.getDocument(sb.toString());
		return doc;
    }
}
