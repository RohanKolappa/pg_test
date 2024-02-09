package com.ipvs.sessionservice.impl;


import org.dom4j.Document;

import com.ipvs.common.IPVSTask;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;


public class RequesterTask extends IPVSTask {

	protected Document requestDoc;
	
	public RequesterTask(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document requestDoc, String ID)
	{
		super(request, xmpp, log, db, resourceFactory, serviceAgentFactory, ID);
		this.requestDoc = requestDoc;
	}
	
	public void run()
	{
		try
		{						
			String requestName = MRXMLUtils.getValueXpath(requestDoc, "//"+MRConst.REQUEST_NAME, "");
			MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), requestDoc, log, requestName, AppServerAgent.class.getName(), request.getAppServerJID());
		}
		catch(Exception e)
		{						
			e.printStackTrace();
		}
	}
	
}
