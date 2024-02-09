package com.ipvs.sessionservice.service.handler;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Produces;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.HttpHeaders;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.HttpRequestHandlerI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.AuthTokenCache.UserResource;
import com.ipvs.sessionservice.impl.SessionServiceConst;


@Deprecated
public class LoginRequestHandler implements HttpRequestHandlerI{

	@Context ServletContext servletContext;
	@POST
	@Consumes({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML})
	@Produces({MediaType.APPLICATION_JSON, MediaType.APPLICATION_XML})
	public Response handleRequest(@Context UriInfo uriInfo,
			@Context HttpHeaders headers) throws Exception{

		AuthTokenCache authTokenCache = null;
		try {
			authTokenCache = AppServerAgentUtils.getAuthTokenCache((ServiceAgentFactoryI)servletContext.getAttribute("ServiceAgentFactoryI"));
			AuthTokenCache.UserResource userResource = null;
			try {
				//String authHeader = headers.getRequestHeaders().get(HttpHeaders.AUTHORIZATION).get(0);
				userResource =  authTokenCache.parseAuthHeader(headers);
			} catch (Exception e) {
			}
			//if user already logged in then return conflict error
			if(userResource!=null) {
				UserResource userExists = authTokenCache.getUserResource(userResource.getUsername());
				if(userExists != null && userExists.isPresenceAvailable()) {
					return Response.status(HttpServletResponse.SC_CONFLICT).entity("User Already Loggedin").build();
				}
			}

			if(userResource==null || authTokenCache.validateUser(userResource)) {
				return Response.status(HttpServletResponse.SC_UNAUTHORIZED).build();
			}

			userResource.setPresenceAvailable(true);
			return Response.ok("User Login").header(SessionServiceConst.REST_TOKEN_HEADER, userResource.getUserJID()).build();
		} catch (ServletException e) {
			e.printStackTrace();
			return Response.status(HttpServletResponse.SC_INTERNAL_SERVER_ERROR).entity(e.toString()).build();
		}

	}
} 
