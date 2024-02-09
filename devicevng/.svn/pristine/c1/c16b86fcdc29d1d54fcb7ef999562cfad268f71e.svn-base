package com.barco.device.vng.DeviceAdmin;

import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;

import javax.net.ssl.HttpsURLConnection;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.handler.DeviceAdmin.SCServiceReadyEventHandler;
import com.barco.device.vng.DeviceAdmin.HAService.ReplicationStatus;
import com.barco.utils.XMLUtils;

public class DownloadDBResponseHandler implements ResponseHandlerI {

	private static final int BUFFER_SIZE = 6 * 1024 * 1024;
	
	@Override
	public void handleResponse(Message response, Document context,
			DeviceI device) throws Exception {
		Document messageDoc = response.getMessageDoc();
		Element urlEl = XMLUtils.getElementXpath(messageDoc, "//URL");      
		if(urlEl == null || urlEl.getTextTrim().isEmpty()) {
			device.getLogger().info("DownloadDBRequestHandler: No URL found!");
			device.getLogger().info("DownloadDBResponseHandler: Resend the downloadDB request.. ");
			HAUtils.sendDownloadDBRequest(device, new DownloadDBResponseHandler());
			return;
		}
		String url = urlEl.getTextTrim();
		device.getLogger().info("DownloadDBResponseHandler: URL received: " + url);
		
		try {
			downloadDB(device.getLogger(), url, HAUtils.DOWNLOAD_DIR_PATH+"/");
			// Start replication
			HAUtils.startReplication(device.getLogger());
			device.getLogger().info("DownloadDBResponseHandler: Replication Done, Send DeviceLogin");

			// Mark replication done
			HAUtils.setReplicationStatus(ReplicationStatus.Done);
			
			// Now send the device login
			new SCServiceReadyEventHandler().sendDeviceLoginRequest(device);
			
		} catch (Exception e) {
			device.getLogger().info("DownloadDBResponseHandler: Error handling the download db response! : " + e.toString());
			device.getLogger().info("DownloadDBResponseHandler: Resend the downloadDB request.. ");
			HAUtils.sendDownloadDBRequest(device, new DownloadDBResponseHandler());
		}
		
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device)
			throws Exception {
		
		device.getLogger().error("DownloadDBResponseHandler: Download DB Response Failed, Attempting to resend the request.." + response.getMessageDoc().asXML());
		
		// Send DownloadDB request to active HA server
		HAUtils.sendDownloadDBRequest(device, new DownloadDBResponseHandler());
	}
	
	private void downloadDB(Logger log, String urlStr, String downloadPath) throws Exception {
		URL url = new URL(urlStr);
		HttpsURLConnection httpConn = (HttpsURLConnection) url.openConnection();
		httpConn.setSSLSocketFactory(SecurityUtils.getTrustServerConnectionFactory());
		httpConn.setHostnameVerifier(SecurityUtils.getHostVerifier());
		
		int responseCode = httpConn.getResponseCode();
		log.info("downloadDB responseCode :"+responseCode);
		if (responseCode == HttpsURLConnection.HTTP_OK) {
			log.info("responseCode OK :"+ responseCode);
			downloadPath+=HAUtils.DOWNLOAD_TGZ_FILE_NAME;
			log.info("downloadPath with extension:" + downloadPath);
			InputStream inputStream = httpConn.getInputStream();
			FileOutputStream outputStream = new FileOutputStream(downloadPath);
			int bytesRead = -1;
			byte[] buffer = new byte[BUFFER_SIZE];
			while ((bytesRead = inputStream.read(buffer)) != -1) {
				outputStream.write(buffer, 0, bytesRead);
			}

			outputStream.close();
			inputStream.close();

		}else{
			log.info("downloadDB Failed! responseCode Error :" + responseCode);
		}

	}

}
