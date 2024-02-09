package com.ipvs.app.http.servlet;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.dom4j.Element;
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


public class MediaDownloadServlet extends BaseMediaServlet{

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public MediaDownloadServlet(XMPPF xf, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, String userJID) {
		super(xf, log, db, resourceFactory, userJID);
	}

	@Override
    protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		try {

			MRRequest request = MRRequest.createRequest(userJID);
			request.setSuperUser(true);
			Document queryResultDoc = getURLQuery(req, resp, request);
			
			String downloadURL = MRXMLUtils.getAttributeValue(queryResultDoc.getRootElement(), SessionServiceResourceConst.URL_ATTRIBUTE, "");
			if (downloadURL.isEmpty()) {
				throw new MRException(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Download URL Not Found");
			}

			String persistence =  req.getParameter("persistence");
			String type =  req.getParameter("type");
			Element ApiKeyTableEl = MRXMLUtils.getElementXpath(queryResultDoc, "//"+SessionServiceResourceConst.API_KEY_TABLE);
			
			downloadURL = updateURLQueryParams(request, downloadURL, queryResultDoc, ApiKeyTableEl);
			
			downloadURL = downloadURL + "&persistence=" + persistence;
			downloadURL = downloadURL + "&type=" + type;
						
			String format =  req.getParameter("format");
			if(!format.isEmpty()) {
                downloadURL = downloadURL + "&format=" + format;
            }
			
			String uuid =  req.getParameter("uuid");
			if(uuid != null && !uuid.isEmpty()) {
                downloadURL = downloadURL + "&uuid=" + uuid;
            }
			
			downloadURL = downloadURL.replaceAll(SessionServiceResourceConst.ACCESS_PATH_CONST, "/mediajs/file/download");
			resp.sendRedirect(downloadURL);

		} catch (MRException exp) {
			resp.setStatus(exp.getCode());
			resp.getWriter().println(exp.toString());
			((Request) req).setHandled(true);
		} catch (Exception e) {
			resp.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
			resp.getWriter().println(MRException.getStackTrace(e));
			((Request) req).setHandled(true);
		}
	}

	@Override
	public void handleAgentResponse(Document agentResponseDoc, Continuation cc) {
	}
}
