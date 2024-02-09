package com.ipvs.common.db;

import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.ws.rs.core.MediaType;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.eclipse.jetty.client.ContentExchange;
import org.eclipse.jetty.client.HttpClient;
import org.eclipse.jetty.io.ByteArrayBuffer;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class DBUtils extends DBUtilsBase {
	public static final String PREFIX_OBJ="#OBJ_";
	public static final String SUFFIX_OBJ="_OBJ#";
	public static final int PREFIX_OBJ_LEN=PREFIX_OBJ.length();
	public static final int SUFFIX_OBJ_LEN=SUFFIX_OBJ.length();

	public static String read(String queryXpath, String serviceName, String objectType, MRLog log) throws Exception{
		MRConfig mrConfig = MRConfig.getInstance(serviceName, objectType, log);
		String collectionName = mrConfig.getParentNIDKey(log).toLowerCase();
		String url = SessionServiceConst.REST_BASEURI + "." + collectionName + SessionServiceConst.REST_QUERYPREFIX + URLEncoder.encode(queryXpath, SessionServiceConst.REST_DEFAULT_CONTENTTYPE);


		HttpClient clientRequest= new HttpClient();
		clientRequest.start();
		ContentExchange contentExchange = new ContentExchange();
		clientRequest.setConnectorType(HttpClient.CONNECTOR_SELECT_CHANNEL);
		contentExchange.setRequestContentType(MediaType.TEXT_XML);
		contentExchange.setMethod("GET");
		contentExchange.setURL(url);
		clientRequest.send(contentExchange);
		contentExchange.waitForDone();
		return contentExchange.getResponseContent();
	}

	public static String readQuery(String query, Map<String, String> pemXPathList, MRDB db, MRLog log) throws Exception{

		query = updateNIDXPathList(query, pemXPathList, log, db);	
		query = convertXPaths( query);
	      /* log.addInfo("########################### QUERY START ###########################");
	       log.addInfo(query);
	       log.addInfo("########################### QUERY END ###########################");*/
	    String url = SessionServiceConst.REST_BASEURI;		
		HttpClient clientRequest= new HttpClient();
		clientRequest.start();
		ContentExchange contentExchange = new ContentExchange();
		clientRequest.setConnectorType(HttpClient.CONNECTOR_SELECT_CHANNEL);
		contentExchange.setMethod("POST");
		contentExchange.setURL(url);
		String queryInput = SessionServiceConst.XML_HEADER +
				"<query xmlns=\"http://exist.sourceforge.net/NS/exist\"> <text>" +
				"<![CDATA[" + query + "]]>" + "</text></query>";

		contentExchange.setRequestContent(new ByteArrayBuffer(queryInput));		  
		clientRequest.send(contentExchange);
		contentExchange.waitForDone();
		String  responseData = contentExchange.getResponseContent();
		if(responseData ==null || responseData.isEmpty()) {
			//close request
			contentExchange.cancel();
			clientRequest.stop();
			
			return "";
		}
		Document responseDoc  = DocumentHelper.parseText(responseData);
		MRXMLUtils.removeNameSpace(responseDoc.getRootElement());
		Element responseElement = MRXMLUtils.getFirstChild(responseDoc.getRootElement());
		if(responseElement == null) {
			//close request
			contentExchange.cancel();
			clientRequest.stop();
			
			return "";
		}
		responseData = MRXMLUtils.elementToString(responseElement);
		
		//close request
		contentExchange.cancel();
		clientRequest.stop();
		
		return responseData;
	}

}
