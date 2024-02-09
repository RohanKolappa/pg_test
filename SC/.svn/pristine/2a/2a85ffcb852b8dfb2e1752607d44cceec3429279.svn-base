package com.barco.nimbus.pubsub.internal;

import java.util.List;

import javax.servlet.http.HttpServletRequest;
import javax.ws.rs.HeaderParam;
import javax.ws.rs.POST;
import javax.ws.rs.Path;
import javax.ws.rs.QueryParam;
import javax.ws.rs.core.Context;
import javax.ws.rs.core.MediaType;
import javax.ws.rs.core.Response;

import com.barco.nimbus.pubsub.api.PubSubHTTPI;

@Path("/jms")
public interface PubSubHTTPJAXRS extends PubSubHTTPI {

	/**
	 * @param session
	 *            your subscription id
	 * @param timeoutMs
	 *            server-side buffer size (in milliseconds)
	 * @param destination
	 *            the jms destination (eg topic/x)
	 * @param selector
	 *            the jms selector (eg method='createDisplay')
	 * @param accept
	 *            "application/json"
	 * @param expression
	 *            the xpath-like bean-expression (eg [0.1=key].3)
	 * @param name
	 *            the value to use in the X-bms-name response header when
	 *            delivering an event
	 * @param suspendSec
	 *            long-poll duration in seconds (after which the call will
	 *            return with 204 "no data")
	 * @param request
	 * @return 201 = subscription initiated, 200 = event delivery, 204 = no
	 *         event happened
	 */
	@POST
	@Path("/get")
	Response getNextMessage(@QueryParam("session") String session,
			@QueryParam("timeoutMs") int timeoutMs,
			@QueryParam("destination") List<String> destination,
			@QueryParam("selector") List<String> selector,
			@QueryParam("accept") MediaType accept,
			@QueryParam("expression") List<String> expression,
			@QueryParam("name") List<String> name,
			@HeaderParam("Accept-Wait") int suspendSec,
			@Context HttpServletRequest request);

	@POST
	@Path("/consumer")
	String subscribe();

	@POST
	@Path("/send") 
	void send(@QueryParam("destination") String destination, @QueryParam("header") List<String> headers, Object body);
}
