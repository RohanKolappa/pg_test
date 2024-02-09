package com.ipvs.device.http.servlet;

import java.io.IOException;
import java.util.Hashtable;
import java.util.Map;
import java.util.UUID;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.util.StringUtil;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.http.server.DeviceHttpServerAgent;
import com.ipvs.http.common.HttpConst;
import com.ipvs.http.common.HttpExceptionCodes;
import com.ipvs.http.servlet.IPVSHttpServlet;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;

@SuppressWarnings("unused")
public class DeviceDiscoveryServlet extends IPVSHttpServlet {

	private static final long serialVersionUID = 1L;

	protected DeviceHttpServerAgent httpServerAgent = null;
	protected DeviceAdminServerAgent deviceAdminServerAgent = null;
	private final String REQUEST_STATE = "REQUEST_STATE";
	private final int WAITING_FOR_RESPONSE_FROM_APPSERVER = 1;
	private final int DONE = 200;
	String localConfig = null;
	Server server = null;
	boolean isServerUp = false;
	Logger logger = null;


	public DeviceDiscoveryServlet(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, DeviceHttpServerAgent httpServerAgent, DeviceAdminServerAgent deviceAdminServerAgent) {
		super(xf, log, db, resourceFactory, userJID);
		this.httpServerAgent = httpServerAgent;
		this.deviceAdminServerAgent = deviceAdminServerAgent;
	}

	protected void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {
		
		String responseXML = "";
		response.setContentType("text/xml");
		
		try {
			responseXML = deviceAdminServerAgent.handleGetDeviceRequest(userJID);
		} catch (Exception e) {
			
			e.printStackTrace();
			response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
			response.getWriter().println();
		}
		if (responseXML != null || !"".equals(responseXML)) {
			response.setStatus(HttpServletResponse.SC_OK);
		}
		response.getWriter().println(responseXML);
		
	}
	
	
	protected void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {

		Document requestDoc = null;
		try {
			StringBuilder sb = new StringBuilder();
			String s = "";
			while ((s = request.getReader().readLine()) != null) {
				sb.append(s);
			}
			requestDoc = DocumentHelper.parseText(sb.toString());
			
			if (requestDoc != null) {
				int agentResponseCode = deviceAdminServerAgent.handleSetDeviceConfigRequest(requestDoc);
				if (agentResponseCode != 0) {
					response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
				} else {
					response.setStatus(DONE);
				}
			}else{
				response.setStatus(HttpServletResponse.SC_BAD_REQUEST);
			}
			
		} catch (Exception e) {
			e.printStackTrace();
			response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
			response.getWriter().println();
		}

		
		response.getWriter().println("<SetDeviceConfigResponseData/>");
	
	}


	@Override
	public void handleAgentResponse(Document agentResponseDoc, Continuation cc) {}

}
