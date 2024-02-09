package com.ipvs.app.http.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.server.Request;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;

public class MediaImageServlet extends BaseMediaServlet{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public MediaImageServlet(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID) {
		super(xf, log, db, resourceFactory, userJID);
	}

	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
	throws ServletException, IOException {
		try {
			
			MRRequest request = MRRequest.createRequest(userJID);
			request.setSuperUser(true);
	    	Document queryResultDoc = getURLQuery(req, resp, request);
	    	
	    	String snapShotURL = MRXMLUtils.getAttributeValue(queryResultDoc.getRootElement(), SessionServiceResourceConst.URL_ATTRIBUTE, "");
			if(snapShotURL.isEmpty()) {
	    		throw new MRException(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Source URL Not Found");
	    	}
	    	
			snapShotURL = updateURLQueryParams(request, snapShotURL, queryResultDoc, MRXMLUtils.getElementXpath(queryResultDoc, "//"+SessionServiceResourceConst.API_KEY_TABLE));
			snapShotURL = snapShotURL.replaceAll(SessionServiceResourceConst.ACCESS_PATH_CONST, "/mediaportal/snapshot");
    		
    		resp.sendRedirect(snapShotURL);
    		
		}catch(MRException exp) {
			exp.printStackTrace();
			resp.setStatus(exp.getCode());
			resp.getWriter().println(exp.toString());			
			((Request)req).setHandled(true);						
		} catch (Exception e) {
			e.printStackTrace();
			resp.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
			resp.getWriter().println(MRException.getStackTrace(e));			
			((Request)req).setHandled(true);
		}		
	}

	@Override
	public void handleAgentResponse(Document agentResponseDoc, Continuation cc) {	
	}

}


