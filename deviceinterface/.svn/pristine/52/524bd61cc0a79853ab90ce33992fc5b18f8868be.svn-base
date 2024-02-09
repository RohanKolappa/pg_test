package com.barco.device.test;
import java.io.IOException;
import java.io.UnsupportedEncodingException;

import org.dom4j.DocumentException;
import org.eclipse.jetty.client.ContentExchange;
import org.eclipse.jetty.client.HttpClient;
import org.eclipse.jetty.http.HttpHeaders;
import org.eclipse.jetty.http.HttpStatus;
import org.eclipse.jetty.io.Buffer;
import org.eclipse.jetty.io.ByteArrayBuffer;


public class ProdataBug {



	static String notificationChannel  = ""; 
	private static String sendHttpRequest(String  url,String requestData,String method) throws Exception {
		HttpClient clientRequest= new HttpClient();
		clientRequest.start();
		ContentExchange contentExchange = new ContentExchange() {

			@Override
			protected synchronized void onResponseHeader(Buffer arg0,
					Buffer arg1) throws IOException {
				// TODO Auto-generated method stub
				super.onResponseHeader(arg0, arg1);
				System.out.println("===================arg0:="+arg0);

				if(arg0.toString().equals("X-Barco-notification-channel")) {
					notificationChannel = arg1.toString();
					System.out.println("=================arg1:="+arg1);
					System.out.println("=================notificationChannel:="+notificationChannel);
				}
			}

			@Override
			protected void onResponseComplete() throws IOException {
				// TODO Auto-generated method stub
				super.onResponseComplete();
				if(getResponseStatus() != HttpStatus.OK_200) {
					//notificationChannel  = "";	
					System.out.println("====================Problem with http response : url:="+getRequestURI() +" status:="+getResponseStatus());					
				}

			}

		};
		clientRequest.setConnectorType(HttpClient.CONNECTOR_SELECT_CHANNEL);
		contentExchange.setMethod(method);
		contentExchange.setURL(url);
		contentExchange.setRequestHeader(HttpHeaders.AUTHORIZATION, "YmFyY286YjRyYzAsQkNE");
		contentExchange.setRequestHeader(HttpHeaders.ACCEPT, "application/xml");
		contentExchange.setRequestHeader("X-Barco-resource", "web-client_barco");					

		contentExchange.setRequestContent(new ByteArrayBuffer(requestData));		  
		clientRequest.send(contentExchange);
		contentExchange.waitForDone();
		String  responseData = contentExchange.getResponseContent();
		contentExchange.cancel();
		clientRequest.stop();	

		System.out.println(responseData);
		return responseData	;	

	}


	/**
	 * @param args
	 */
	public static void main(String[] args) {

		try {
			String url = "http://10.51.51.65:10088/controller/Service/NotificationChannel?token=NzcyNjcwRTMtRjY0Qi00MkVCLTgwNDctRjhGNkFEODY5RkY0fHNyY3VzZXIx";		


			String queryInput = "<ServiceLogin clientVersion=\"2.31.14\">" +
					"    <clientPrivateKey></clientPrivateKey>" +
					"    <clientTemplateTitle>web-client</clientTemplateTitle></ServiceLogin>";


			//login 

			while(true) {
				while(true) {
					url = "http://10.51.51.78/controller/Service/Login";		


					queryInput = "<ServiceLogin clientVersion=\"2.31.14\">" +
							"    <clientPrivateKey></clientPrivateKey>" +
							"    <clientTemplateTitle>web-client</clientTemplateTitle></ServiceLogin>";
					String responseData = sendHttpRequest(url, queryInput,"POST");	
					System.out.println("responseData :="+responseData);
					if(!notificationChannel.isEmpty()) {
						break;
					}
					System.out.println("======================Retry login after 30000 milliseconds ");
				//	Thread.currentThread().sleep(30000);
				}


				while(true) {
					sendHttpRequest(notificationChannel, "", "GET");
					System.out.println("================================notificationChannel :="+notificationChannel);
				}

//				url = "http://10.51.51.78/controller/Service/Logout";	
//				sendHttpRequest(url, "", "POST");
			}
			//							
			//			}


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

}
