package com.barco.nimbus.displaymanagement.bms;

import java.util.List;

import javax.ws.rs.Consumes;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.Request;
import javax.ws.rs.core.Response;
import javax.ws.rs.core.UriInfo;

@Path("/NetworkWall")
public interface BMSEventSubscriptionServiceI {
	
	/**
	 * @param name
	 *            the list of preconfigured names is defined in
	 *            com.barco.compose.apibean.NetvizEventService.properties
	 * @return a 201 with a Location header containing the long-polling url
	 *         (with all query parameters filled).
	 */
	@POST
	@Path("/event")
	Response subscribe(
			@QueryParam("name") List<String> name, 
			@QueryParam("accept") String accept,
			@QueryParam("room") String room,
			@QueryParam("sink") String sink,
			@QueryParam("selector-session") String session,
			@QueryParam("expression") String expression,
			@Context UriInfo uri,
			@Context Request request);
}
