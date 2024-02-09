package com.ipvs.sessionservice.service.handler;

import javax.servlet.ServletContext;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;

import com.ipvs.common.HttpRequestHandlerI;
import com.ipvs.http.servlet.BaseHtppRequestHandler;

@Path("/Service/Logout")
public class LogoutRequestHandler extends BaseHtppRequestHandler implements HttpRequestHandlerI{

	@Context ServletContext servletContext;
	
	@POST
	@Produces({ MediaType.APPLICATION_XML, MediaType.APPLICATION_JSON, MediaType.TEXT_HTML })
	public Response handleRequest(@Context UriInfo uriInfo,
			@Context HttpHeaders headers) throws Exception{
		setInfo(servletContext);
		return handleUserUnavailable(headers, servletContext, false);
	}

	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		// TODO Auto-generated method stub
		return null;
	}
	

}
