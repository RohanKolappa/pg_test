package com.ipvs.sessionservice.session.handler;




import javax.jws.WebService;
import javax.servlet.ServletContext;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import org.dom4j.Document;
import org.jboss.resteasy.annotations.Suspend;
import org.jboss.resteasy.spi.AsynchronousResponse;
import org.jboss.resteasy.util.HttpResponseCodes;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.http.servlet.BaseHtppRequestHandler;

@Path("/AuthenticateURL")
@WebService(name="AuthenticateURLRequestHandler")
public class AuthenticateURLRequestHandler extends BaseHtppRequestHandler{

	public AuthenticateURLRequestHandler() {
		
	}
	
	@Context ServletContext servletContext;
	@Context HttpServletRequest servletRequest;
	@Context HttpServletResponse servletResponse;
	@POST
	public void handlePostRequest(String xml, @Context UriInfo uriInfo, @Context HttpHeaders headers, @QueryParam("userJID") String userJID, @QueryParam("signature") String signature ,final @Suspend(5000) AsynchronousResponse asynResponse) throws MRException{
		setInfo(servletContext);
		/*Response response = Response.status(HttpResponseCodes.SC_ACCEPTED).type(MediaType.TEXT_XML).entity("Accepted Thumbnail Request!").build();
		asynResponse.setResponse(response);*/
	
		try {
			HttpAuthentication.validateSignature(servletRequest, servletResponse, xmpp, serviceAgentFactory, db, log, false);
			//log.addInfo("AuthenticateURLRequestHandler: URL Authenticated sending "+HttpResponseCodes.SC_OK);			
			Response response = Response.status(HttpResponseCodes.SC_OK).type(MediaType.TEXT_XML).entity("URL Is Authenticated!").build();
			asynResponse.setResponse(response);	
		}
		catch (MRException e) {
			e.printStackTrace();
			Response response = Response.status(e.getCode()).type(MediaType.TEXT_XML).entity("URL Is not Authenticated!").build();
			asynResponse.setResponse(response);
			log.addInfo("Unauthorized URL for"+ userJID + e.toString() ,MRLog.ERROR,MRLog.NOTIFY);
		}
		catch (Exception e) {
			e.printStackTrace();
			Response response = Response.status(HttpResponseCodes.SC_INTERNAL_SERVER_ERROR).type(MediaType.TEXT_XML).entity("Internal Server Error!").build();
			asynResponse.setResponse(response);
			log.addInfo("Error in Authenticate URL Request for"+ userJID + e.toString() ,MRLog.ERROR,MRLog.NOTIFY);
		}
		
	}
	
	@Override
	protected String getResponseString(Document responseDoc, String requestName)
			throws Exception {
		return "";
	}
	
}
