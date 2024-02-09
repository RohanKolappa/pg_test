package com.ipvs.sessionservice.impl;


import java.util.StringTokenizer;

import org.dom4j.Document;

import com.ipvs.common.IPVSScheduler;
import com.ipvs.common.IPVSTask;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;


public class ScheduleManager {
		
	public void addTaskToScheduler(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, String exportSchedule, String NID, Document requestDoc) throws Exception
	{
		StringTokenizer token = new StringTokenizer(exportSchedule.trim(), ",");		
		while (token.hasMoreTokens()) 
		{
			IPVSTask task = new RequesterTask(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, NID);							
			IPVSScheduler.getInstance().scheduleDaily(task, Integer.parseInt((token.nextToken()).trim()));
		}
	}
	
	public void deleteTaskFromScheduler(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, String NID) throws Exception
	{											
		StringTokenizer token = new StringTokenizer(NID, ",");
		while(token.hasMoreTokens())
		{
			IPVSScheduler.getInstance().removeTask(token.nextToken());
		}
	}
	
}