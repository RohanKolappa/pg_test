package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;

public class ViewRequestHandler extends UpdateSessionRequestHandler{

	protected void addSourcesAndConnections(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			Document startSessionRequestDoc, String sessionId, String connId, Element viewBaseElement, String dstEntityId) throws Exception {
		Element entity = UpdateSessionUtils.createSrcPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId);
		String srcEntityId = entity.attributeValue(SessionServiceConst.ID);
		Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add((Element)entity.clone());
		
		String connName = viewBaseElement.attributeValue(SessionServiceConst.NAME, "");
	    String canvasId = viewBaseElement.attributeValue(SessionServiceConst.CANVASID, "");
	    String windowId = viewBaseElement.attributeValue(SessionServiceConst.WINDOWID, "");

		Element connection = this.createConnection(connId, dstEntityId, srcEntityId, connName, startSessionRequestDoc, canvasId, windowId);
		Element connectionList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.CONNECTIONLIST);
		connectionList.add((Element)connection.clone());
	}
	
	

	protected String addSrcEntityToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID) throws Exception {		
		return UpdateSessionUtils.addSrcPortToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);
	}
	
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		return this.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "View");
	}


}
