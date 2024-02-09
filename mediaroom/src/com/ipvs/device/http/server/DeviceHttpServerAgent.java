package com.ipvs.device.http.server;

import java.util.HashSet;
import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import org.eclipse.jetty.servlet.ServletMapping;

import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.http.servlet.DeviceDiscoveryServlet;
import com.ipvs.device.http.servlet.DeviceSnapshotServlet;
import com.ipvs.http.server.IPVSHttpServer;
import com.ipvs.http.servlet.IPVSHttpServlet;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;

public class DeviceHttpServerAgent implements ServiceAgentI{	

	private IPVSHttpServer httpServer = null;

	private XMPPF xf;
	private MRLog log;
	private MRDB db;
	private MRResourceFactoryI resourceFactory;
	protected String userJID;  
	private final static String SNAPSHOT_SERVLET_NAME = "SnapshotServlet";
	private final static String DEVICE_DISCOVERY_SERVLET_NAME = "DeviceDiscoveryServlet";
	
	private Hashtable<String,ServletCache> servletCache = null;

    public void addServlettoCache(String clientdata,IPVSHttpServlet httpServlet,Continuation cc) {
		ServletCache cahcedObject = new ServletCache(httpServlet,cc);
		servletCache.put(clientdata, cahcedObject);
	}
	
	public ServletCache removeServletFromCache(String clientdata) {
		return servletCache.remove(clientdata);
	}
	
	
	public ServletCache getServletCacheObject(String clientdata) {
		return servletCache.get(clientdata);
	}


	public DeviceHttpServerAgent(int port, MRLog log) {
		httpServer = new IPVSHttpServer(port, log);	
	}

	public void startServer(DeviceAdminServerAgent deviceAdminServerAgent)
	{			
		ServletContextHandler servletContextHandler = new ServletContextHandler(httpServer,"/");
		httpServer.setHandler(servletContextHandler);	
		servletContextHandler.addServlet(new ServletHolder(DEVICE_DISCOVERY_SERVLET_NAME, new DeviceDiscoveryServlet(xf,  log,  db,  resourceFactory,
				userJID, this, deviceAdminServerAgent)), "/deviceinterface/*");
		httpServer.startServer();	
		servletCache = new Hashtable<String,ServletCache>();
	}
	
	public void stopServer() throws Exception {
		httpServer.stop();
	}

	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			String userJID, Document setServiceDomainResponseDoc) throws Exception {
		this.xf = xf;
		this.log = log;
		this.db = db;
		this.resourceFactory = resourceFactory;
		this.userJID = userJID;  
		ServletContextHandler sch= (ServletContextHandler)httpServer.getHandler();
		sch.addServlet(new ServletHolder(SNAPSHOT_SERVLET_NAME, new DeviceSnapshotServlet(xf,  log,  db,  resourceFactory, userJID, this)), "/device/snapshot");
		log.addInfo("DeviceHttpServerAgent : Agent Intialized send AgentStateEvent ", MRLog.INFO, MRLog.NOTIFY);
		ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), DeviceHttpServerAgent.class.getName(), MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);	
	
	}

	@Override
	public void reset() throws Exception {
		ServletContextHandler sch = (ServletContextHandler) httpServer.getHandler();
		// Remove Snapshot Servlet
		ServletHandler servletHandler = sch.getServletHandler();
		ServletMapping[] servletMappings = servletHandler.getServletMappings();
		HashSet<ServletMapping> newServletMappings = new HashSet<ServletMapping>();
		for (ServletMapping servletMapping : servletMappings) {
			if (DEVICE_DISCOVERY_SERVLET_NAME.equals(servletMapping.getServletName())) {
				newServletMappings.add(servletMapping);
			}
		}
		servletHandler.setServletMappings(newServletMappings.toArray(new ServletMapping[]{}));
		this.userJID = "";
	}

	@Override
	public boolean handleEvent(MRClient client, Document doc, Element event)
	throws Exception {		
		return false;
	}

	@Override
	public String handleMessage(MRClient client, String fromJID, String message)
	throws Exception {

		Document messageDoc = null;
		messageDoc = MRXMLUtils.stringToDocument(message);

		String messagetype = MRXMLUtils.getAttributeValueXpath(messageDoc, "//AgentMessage","type", "");

		if (!ServiceClient.RESPONSE.equals(messagetype)) {
			return message;
		} 

		String requestName = MRXMLUtils.getAttributeValueXpath(messageDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
	
		try {
			if (requestName.isEmpty()) {
				return null;
			}

			
			Document responseDoc = MRXMLUtils.getDocumentXpath(messageDoc, "//" + ServiceClient.RESPONSE);
			String clientdata = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
			
			log.addInfo("DeviceHttpServerAgent: Received Response. RequestName:=" + requestName + " From UserJID:=" + fromJID, MRLog.INFO, MRLog.NOTIFY);
		    ServletCache cacheObject= getServletCacheObject(clientdata);
		    if(cacheObject == null) {
		    	log.addInfo("DeviceHttpServerAgent: Servlet not found for RequestName:=" + requestName , MRLog.INFO, MRLog.NOTIFY);
		    	return message;
		    }
		    removeServletFromCache(clientdata);
		    cacheObject.httpServlet.handleAgentResponse(responseDoc, cacheObject.cc);
			
		} catch (MRException exp) {
			exp.printStackTrace();
		} catch (Exception exp) {
			exp.printStackTrace();
		}

		return null;
	}

}

class ServletCache{
	IPVSHttpServlet httpServlet = null;
	Continuation cc=null;
	public ServletCache(IPVSHttpServlet httpServlet, Continuation cc) {
		this.httpServlet = httpServlet;
		this.cc=cc;
	}
}


