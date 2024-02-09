import java.io.IOException;
import java.io.UnsupportedEncodingException;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.eclipse.jetty.client.ContentExchange;
import org.eclipse.jetty.client.HttpClient;
import org.eclipse.jetty.http.HttpHeaders;
import org.eclipse.jetty.io.ByteArrayBuffer;

import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class Test {


	/**
	 * @param args
	 */
	public static void main(String[] args) {
//		IPVSHttpServer httpServer = new IPVSHttpServer(10080,MRLog.getInstance(MRLog.CONSOLE));	
//
//
//		ContextHandlerCollection contexts = new ContextHandlerCollection();
//	
//		MovedContextHandler moved = new MovedContextHandler(contexts,"/mediaroom/snapshottest","http://10.51.51.9/mediaportal/testsnapshot");
//		moved.setDiscardPathInfo(true);	
//	
//		ServletContextHandler servletContextHandler = new ServletContextHandler(contexts,"/mediaroom");
//		//httpServer.setHandler(servletContextHandler);
//		servletContextHandler.addServlet(new ServletHolder(new MediaImageServlet(null,null,null,null,null)),"/snapshot1");
//		
//		httpServer.setHandler(contexts);
//		httpServer.startServer();
//		
//		try {
//			httpServer.join();
//		} catch (InterruptedException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
		
		
	    try {
			String url = "http://10.51.51.65:10088/controller/Service/NotificationChannel?token=NzcyNjcwRTMtRjY0Qi00MkVCLTgwNDctRjhGNkFEODY5RkY0fHNyY3VzZXIx";		
			
						
			String queryInput = "<ServiceLogin clientVersion=\"2.31.6\">" +
					"    <clientPrivateKey>Ipvs1234</clientPrivateKey>" +
					"    <clientTemplateTitle>wc1</clientTemplateTitle></ServiceLogin>";
			
			String responseData = sendHttpRequest(url, "");	
			Document responsedoc = MRXMLUtils.stringToDocument(responseData);
			String portTitle = MRXMLUtils.getAttributeValueXpath(responsedoc,"//Port","title", "");
			String presence = MRXMLUtils.getAttributeValueXpath(responsedoc,"//Port","presence", "");
			System.out.println("portTitle:="+portTitle);
			System.out.println("presence:="+presence);
            String nextDeviceIndex = StringUtils.substringBetween(portTitle, "devicea_", "_CHANNEL");
            System.out.println("nextDevicEIndex:="+nextDeviceIndex);
            int nextIndex = Utils.getIntValue(nextDeviceIndex, 0);    
            String nextPresence = presence;
            int counter =1;
			while (true) {
				List<String> portTitleVales = MRXMLUtils.getValuesXpath(responsedoc, "//Port/@title");
				List<String> portPresenceValues = MRXMLUtils.getValuesXpath(responsedoc, "//Port/@presence");
				System.out.println("portTitleVales:="+portTitleVales);
				System.out.println("portPresenceValues:="+portPresenceValues);
				//url = "http://10.51.51.65/controller/Service/NotificationChannel?token=OTMwMTlFMDktOTIyNi00MUJGLTgzQkMtQkRDQ0Q0ODBEMTQzfHNyY3VzZXIx&counter="+counter++;		
				
	           if(portTitleVales == null || portPresenceValues == null) {
	        	   continue;
	           }
				for(int i=0;i<portTitleVales.size();i++) {
					
				    int deviceIndex = Utils.getIntValue(StringUtils.substringBetween(portTitleVales.get(i), "devicea_", "_CHANNEL"),0);
				    String devicePresence = portPresenceValues.get(i);
				    if(deviceIndex != nextIndex && !devicePresence.equals(nextPresence)) {
				    	System.out.println("TEST FAILED=================== expecting device index :="+nextIndex + " got index:="+deviceIndex+" Expecting presence:="+nextPresence+" got presencE:="+devicePresence);
				    	System.exit(0);
				    }
				    System.out.println("TEST PASSED ================== expecting device index :="+nextIndex + " got index:="+deviceIndex +" Expecting presence:="+nextPresence+" got presencE:="+devicePresence);
			    	
				    if(nextPresence.equals("available")){
				    	nextPresence="unavailable";
				    } else {
				    	nextIndex++;
				    	if(nextIndex > 100) {
				    		nextIndex = 1;
				    	}
				    	nextPresence="available";				    	
				    }
			         
				}
				
				responseData = sendHttpRequest(url, "");	
				responsedoc = MRXMLUtils.stringToDocument(responseData);
				
				
			}
			
			
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		} catch (DocumentException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}
	private static String sendHttpRequest(String  url,String requestData) throws Exception {
		HttpClient clientRequest= new HttpClient();
		clientRequest.start();
		ContentExchange contentExchange = new ContentExchange();
		clientRequest.setConnectorType(HttpClient.CONNECTOR_SELECT_CHANNEL);
		contentExchange.setMethod("GET");
		contentExchange.setURL(url);
		contentExchange.setRequestHeader(HttpHeaders.AUTHORIZATION, "c3JjdXNlcjE6SXB2czEyMzQK");
		contentExchange.setRequestHeader(HttpHeaders.ACCEPT, "application/xml");
		contentExchange.setRequestHeader("X-Barco-resource", "wc1_srcuser1");					
	
		contentExchange.setRequestContent(new ByteArrayBuffer(requestData));		  
		clientRequest.send(contentExchange);
		contentExchange.waitForDone();
	
		String  responseData = contentExchange.getResponseContent();
		System.out.println(clientRequest.getResponseHeaderType());
		contentExchange.cancel();
		clientRequest.stop();	
		
		System.out.println(responseData);
		return responseData	;	

	}

}
