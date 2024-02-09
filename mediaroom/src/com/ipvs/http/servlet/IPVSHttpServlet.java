package com.ipvs.http.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.eclipse.jetty.continuation.Continuation;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;

public abstract class IPVSHttpServlet extends HttpServlet{
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	protected static final long CONTINUATION_TIME_OUT =5000;
	protected static final String RESULTS ="results";
	protected XMPPF xf;
	protected MRLog log;
	protected MRDB db;
	protected MRResourceFactoryI resourceFactory;
	protected String userJID; 
	
	public IPVSHttpServlet(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			String userJID) {
		this.xf = xf;
		this.log = log;
		this.db = db;
		this.resourceFactory = resourceFactory;
		this.userJID = userJID;  
	}
	
	/* (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doGet(javax.servlet.http.HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		super.doGet(req, resp);
	}

	/* (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#doPost(javax.servlet.http.HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		super.doPost(req, resp);
	}

	/* (non-Javadoc)
	 * @see javax.servlet.http.HttpServlet#service(javax.servlet.http.HttpServletRequest, javax.servlet.http.HttpServletResponse)
	 */
	@Override
	protected void service(HttpServletRequest arg0, HttpServletResponse arg1)
			throws ServletException, IOException {
		super.service(arg0, arg1);
	}
	
	public abstract void handleAgentResponse(Document agentResponseDoc,Continuation cc);
}
