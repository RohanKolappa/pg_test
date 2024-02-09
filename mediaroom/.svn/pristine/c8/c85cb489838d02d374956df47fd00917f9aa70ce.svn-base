package com.ipvs.common;


import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;


public abstract class IPVSTask implements Runnable {

	protected MRRequest request;
	protected XMPPI xmpp;
	protected MRLog log;
	protected MRDB db;
	protected MRResourceFactoryI resourceFactory;
	protected ServiceAgentFactoryI serviceAgentFactory;	
	protected String taskID;
		
	public IPVSTask(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, String taskID)
	{
		this.request = request;
		this.xmpp = xmpp;
		this.log = log;
		this.db = db;
		this.resourceFactory = resourceFactory;
		this.serviceAgentFactory = serviceAgentFactory;		
		this.taskID = taskID;	
	}
	
	public String getTaskID()
	{
		return taskID;
	}
			
}
